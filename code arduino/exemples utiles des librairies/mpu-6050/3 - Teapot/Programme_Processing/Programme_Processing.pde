import processing.serial.*;
import processing.opengl.*;
import toxi.geom.*;
import toxi.processing.*;

ToxiclibsSupport gfx;
Serial port;          
char[] teapotPacket = new char[14]; 
int serialCount = 0;            
int aligned = 0;
int interval = 0;
float[] q = new float[4];
Quaternion quat = new Quaternion(1, 0, 0, 0);
float[] gravity = new float[3];
float[] euler = new float[3];
float[] ypr = new float[3];

void setup() {
    size(300, 300, OPENGL);
    gfx = new ToxiclibsSupport(this);
    lights();
    smooth();
    //================================================================
    //================================================================
    port = new Serial(this, "COM3", 38400);// <= Editez votre port COM
    //================================================================
    //================================================================
    port.write('r');
}

void draw() {
    if (millis() - interval > 1000) {
        port.write('r');
        interval = millis();
    }
    background(0);
    pushMatrix();
    translate(width / 2, height / 2);
    float[] axis = quat.toAxisAngle();
    rotate(axis[0], -axis[1], axis[3], axis[2]);

    fill(255, 0, 0, 200);
    box(10, 10, 200);

    fill(0, 0, 255, 200);
    pushMatrix();
    translate(0, 0, -120);
    rotateX(PI/2);
    drawCylinder(0, 20, 20, 8);
    popMatrix();

    fill(0, 255, 0, 200);
    beginShape(TRIANGLES);
    vertex(-100,  2, 30); vertex(0,  2, -80); vertex(100,  2, 30);  
    vertex(-100, -2, 30); vertex(0, -2, -80); vertex(100, -2, 30);  
    vertex(-2, 0, 98); vertex(-2, -30, 98); vertex(-2, 0, 70);  
    vertex( 2, 0, 98); vertex( 2, -30, 98); vertex( 2, 0, 70);  
    endShape();
    beginShape(QUADS);
    vertex(-100, 2, 30); vertex(-100, -2, 30); vertex(  0, -2, -80); vertex(  0, 2, -80);
    vertex( 100, 2, 30); vertex( 100, -2, 30); vertex(  0, -2, -80); vertex(  0, 2, -80);
    vertex(-100, 2, 30); vertex(-100, -2, 30); vertex(100, -2,  30); vertex(100, 2,  30);
    vertex(-2,   0, 98); vertex(2,   0, 98); vertex(2, -30, 98); vertex(-2, -30, 98);
    vertex(-2,   0, 98); vertex(2,   0, 98); vertex(2,   0, 70); vertex(-2,   0, 70);
    vertex(-2, -30, 98); vertex(2, -30, 98); vertex(2,   0, 70); vertex(-2,   0, 70);
    endShape();
    popMatrix();
}

void serialEvent(Serial port) {
    interval = millis();
    while (port.available() > 0) {
        int ch = port.read();
        print((char)ch);
        if (aligned < 4) {             
               if (serialCount == 0) {                 
                     if (ch == '$') aligned++; else aligned = 0;             
                     } else if (serialCount == 1) {                 
                     if (ch == 2) aligned++; else aligned = 0;             
                     } else if (serialCount == 12) {                 
                     if (ch == '\r') aligned++; else aligned = 0;             
                     } else if (serialCount == 13) {                 
                     if (ch == '\n') aligned++; else aligned = 0;             
                     }             
               serialCount++;             
        if (serialCount == 14) serialCount = 0;         
        } else {             
                     if (serialCount > 0 || ch == '$') {
                     teapotPacket[serialCount++] = (char)ch;
                     if (serialCount == 14) {
                     serialCount = 0; 

                     q[0] = ((teapotPacket[2] << 8) | teapotPacket[3]) / 16384.0f;
                     q[1] = ((teapotPacket[4] << 8) | teapotPacket[5]) / 16384.0f;
                     q[2] = ((teapotPacket[6] << 8) | teapotPacket[7]) / 16384.0f;
                     q[3] = ((teapotPacket[8] << 8) | teapotPacket[9]) / 16384.0f;
                     for (int i = 0; i < 4; i++) if (q[i] >= 2) q[i] = -4 + q[i];

                    quat.set(q[0], q[1], q[2], q[3]);
                }
            }
        }
    }
}

void drawCylinder(float topRadius, float bottomRadius, float tall, int sides) {
    float angle = 0;
    float angleIncrement = TWO_PI / sides;
    beginShape(QUAD_STRIP);
    for (int i = 0; i < sides + 1; ++i) {
        vertex(topRadius*cos(angle), 0, topRadius*sin(angle));
        vertex(bottomRadius*cos(angle), tall, bottomRadius*sin(angle));
        angle += angleIncrement;
    }
    endShape();

    if (topRadius != 0) {
        angle = 0;
        beginShape(TRIANGLE_FAN);

        vertex(0, 0, 0);
        for (int i = 0; i < sides + 1; i++) {
            vertex(topRadius * cos(angle), 0, topRadius * sin(angle));
            angle += angleIncrement;
        }
        endShape();
    }

    if (bottomRadius != 0) {
        angle = 0;
        beginShape(TRIANGLE_FAN);
        vertex(0, tall, 0);
        for (int i = 0; i < sides + 1; i++) {
            vertex(bottomRadius * cos(angle), tall, bottomRadius * sin(angle));
            angle += angleIncrement;
        }
        endShape();
    }
}
