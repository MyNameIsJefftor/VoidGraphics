#pragma once

class oldcube {
public:
	float* verts;
	int index[36] = {
		//top
		1, 2, 4,
		2, 3, 4,
		//front
		2, 3, 7,
		2, 6, 7,
		//right
		4, 3, 7,
		4, 8, 7,
		//left
		2, 1, 5,
		2, 6, 5,
		//bottom
		7, 6, 5,
		7, 8, 5,
		//back
		4, 1, 5,
		4, 8, 5
	};
	oldcube(float color[], float scale = 1.0f) {
		float temp[48] = {
			//Pos				//color
		//1
			-0.5f * scale,  0.5f * scale, -0.5f * scale, color[0], color[1],color[2],
		//2	    
			-0.5f * scale,  0.5f * scale,  0.5f * scale, color[0], color[1],color[2],
		//3	    
			0.5f * scale,  0.5f * scale,  0.5f * scale, color[0], color[1],color[2],
		//4	    
			0.5f * scale,  0.5f * scale, -0.5f * scale, color[0], color[1],color[2],
		//5
			-0.5f * scale, -0.5f * scale, -0.5f * scale, color[0], color[1],color[2],
		//6	   
			-0.5f * scale, -0.5f * scale,  0.5f * scale, color[0], color[1],color[2],
		//7	   
			0.5f * scale, -0.5f * scale,  0.5f * scale, color[0], color[1],color[2],
		//8	   
			0.5f * scale, -0.5f * scale, -0.5f * scale, color[0], color[1],color[2]
		};
		verts = temp;
	}
};
class newcube {

};
class testTriangle {
public:
	float verts[18] = {
		// positions			// colors
		 0.000f,  0.1f, 0.0f,	1.0f, 0.0f, 0.0f,   // top
		-0.125f, -0.1f, 0.0f,	0.0f, 1.0f, 0.0f,   // left
		 0.125f, -0.1f, 0.0f,	0.0f, 0.0f, 1.0f    // right 
	};
	testTriangle() {

	};
	testTriangle(float color[], float scale = 1.0f) {
		verts[0] *= scale;
		verts[1] *= scale;
		verts[2] *= scale;

		verts[3] = color[0];
		verts[4] = color[1];
		verts[5] = color[2];

		verts[6] *= scale;
		verts[7] *= scale;
		verts[8] *= scale;

		verts[9] = color[0];
		verts[10] = color[1];
		verts[11] = color[2];

		verts[12] *= scale;
		verts[13] *= scale;
		verts[14] *= scale;

		verts[15] = color[0];
		verts[16] = color[1];
		verts[17] = color[2];
	}
};
class BaseTriangle {

};