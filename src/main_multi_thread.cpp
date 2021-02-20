#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <thread>
#include <atomic>

// reload shared VBO with random data every second
void MyThread(GLFWwindow* win, GLuint vbo, std::atomic< bool >& running)
{
    glfwMakeContextCurrent(win);
    glewInit();
    while (running)
    {
        float temp[512];
        for (size_t i = 0; i < 512; i += 2)
        {
            temp[i + 0] = static_cast<float>(rand() % 600);
            temp[i + 1] = static_cast<float>(rand() % 600);
        }

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(temp), temp, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // See GL 3.3 spec, section D.3.1
        glFinish();

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

int main(int argc, char** argv)
{
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    GLFWwindow* threadWin = glfwCreateWindow(1, 1, "Thread Window", NULL, NULL);

    glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
    GLFWwindow* window = glfwCreateWindow(600, 600, "Hello World", NULL, threadWin);
    glfwMakeContextCurrent(window);
    glewInit();

    // load shared VBO with dummy data
    float temp[512] = { 0 };
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 512, temp, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    std::atomic< bool > running(true);
    std::thread aThread(MyThread, threadWin, vbo, std::ref(running));

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);

        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, 600, 0, 600, -1, 1);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // re-bind shared VBO to pick up potential content changes
        // See GL 3.3 spec, section D.3.3, Rule 4
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_FLOAT, 0, 0);
        glColor3ub(255, 0, 0);
        glDrawArrays(GL_LINES, 0, 256);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        glfwSwapBuffers(window);
    }

    running = false;
    aThread.join();

    glfwTerminate();
    return 0;
}