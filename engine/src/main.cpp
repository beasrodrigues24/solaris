#ifdef __APPLE__
#    include <GLUT/glut.h>
#else
#    include <GL/glut.h>
#endif

#include "logger.hpp"
#include "parse_error.hpp"
#include "parser.hpp"
#include "scene/world.hpp"

// singletons
World WORLD;
int CENTER_X;
int CENTER_Y;

void change_size(int w, int h) {
    CENTER_X = w / 2;
    CENTER_Y = h / 2;

    // Prevent a divide by zero, when window is too short
    // (you cant make a window with zero width).
    if (h == 0)
        h = 1;

    // Set the projection matrix as current
    glMatrixMode(GL_PROJECTION);
    // Load Identity Matrix
    glLoadIdentity();

    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

    // Set perspective
    WORLD.camera.set_prespective(w, h);

    // return to the model view matrix mode
    glMatrixMode(GL_MODELVIEW);
}

void draw_axis() {
    // X axis in red
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(100.0f, 0.0f, 0.0f);
    // Y Axis in Green
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 100.0f, 0.0f);
    // Z Axis in Blue
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 100.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
}

void render_scene(void) {
    // clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set the camera
    WORLD.camera.place();

    if (WORLD.camera.mode() == CameraMode::FPV) {
        glutWarpPointer(CENTER_X, CENTER_Y);
    }

    // draw axis
    // glBegin(GL_LINES);
    // draw_axis();
    // glEnd();

    // draw groups
    // glPolygonMode(GL_FRONT, GL_LINE);
    WORLD.group.draw();

    // end of frame
    glutSwapBuffers();
}

void process_keys(unsigned char key, int x, int y) {
    WORLD.camera.react_key(key, x, y);
    glutPostRedisplay();
}

void cursor_motion(int x, int y) {
    WORLD.camera.cursor_motion(CENTER_X, CENTER_Y, x, y);
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    auto world_res = parse(argv[1]);
    if (world_res.has_error()) {
        error(error_msg(world_res.error()));
        return 1;
    }

    WORLD = world_res.value();

    // init GLUT and the window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 800);
    glutCreateWindow("solaris");

    // Required callback registry
    glutDisplayFunc(render_scene);
    // glutIdleFunc(render_scene);
    glutPassiveMotionFunc(cursor_motion);
    glutMotionFunc(cursor_motion);
    glutReshapeFunc(change_size);

    // Callback registration for keyboard processing
    glutKeyboardFunc(process_keys);
    // glutSpecialFunc(process_special_keys);

    //  OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // enter GLUT's main cycle
    glutMainLoop();

    return 0;
}
