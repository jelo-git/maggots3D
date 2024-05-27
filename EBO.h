#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include <GL/glew.h>

class EBO {
public:
	GLuint id;
	EBO(GLuint* indices, GLsizeiptr size);
	void Bind();
	void Unbind();
	void Delete();
};

#endif // !EBO_CLASS_H