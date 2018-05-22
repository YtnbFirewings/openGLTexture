//
// Created by 唐宇 on 2018/5/20.
//

#include "pic_preview_texture.h"
#define LOG_TAG "PicPreviewTexture"


bool PicPreviewTexture::createTexture() {
    LOGI("enter PicPreviewTexture::createTexture");
    texture=0;
    int ret=initTexture();
    if (ret < 0) {
        LOGI("init texture failed...");
        this->dealloc();
        return false;
    }
    return true;
}

int PicPreviewTexture::initTexture() {
    glGenTextures(1,&texture);
    glBindTexture(GL_TEXTURE_2D,texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D,0);
    return 1;
}

bool PicPreviewTexture::checkGlError(const char *op) {
    GLint  error;
    for (error = glGetError(); error; error = glGetError()) {
        LOGI("error::after %s() glError (0x%x)\n", op, error);
        return true;
    }

    return false;
}

PicPreviewTexture::PicPreviewTexture() {


}

PicPreviewTexture::~PicPreviewTexture() {

}

void PicPreviewTexture::updateTexImage(byte *pixel, int width, int height) {
    if(pixel){
        //TODO cancel
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,texture);
        if(checkGlError("glBindTexture ")){
            return;
        }
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,pixel);
    }

}

bool PicPreviewTexture::bindTexture(GLint uniformaSampler) {
    //激活纹理单元，opengl有多个纹理单元，激活之后绑定纹理到对应的纹理单元
    //然后将纹理单元位置值传入着色器
    //GL_TEXTURE0 对应就是0  一次类推
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,texture);
    if (checkGlError("glBindTexture 111")) {
        return false;
    }
    glUniform1i(uniformaSampler,0);
    return true;
}

void PicPreviewTexture::dealloc() {
    LOGI("enter PicPreviewTexture::dealloc");
    if(texture){
        glDeleteTextures(1,&texture);
    }
    if(render){
        glDeleteRenderbuffers(1,&render);
    }

    if(frame){
        glDeleteFramebuffers(1,&frame);
    }

}

bool PicPreviewTexture::createFramBuffer(const int width, const int height) {
    glGenFramebuffers(1,&frame);
    glGenRenderbuffers(1,&render);
    glBindRenderbuffer(GL_RENDERBUFFER,render);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT16,width,height);
    glBindRenderbuffer(GL_RENDERBUFFER,0);
    glBindFramebuffer(GL_FRAMEBUFFER,frame);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,texture,0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,render);
    // check FBO status
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE){
        LOGI("framebuffer init failed %d",glGetError());
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    return true;
}

bool PicPreviewTexture::bindFrameBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER,frame);
    return true;
}