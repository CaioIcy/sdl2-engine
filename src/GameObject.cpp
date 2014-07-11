#include "GameObject.h"
#include "Game.h"
#include "SDLWrapper.h"
#include "Logger.h"

GameObject::GameObject(const double x_, const double y_, const std::string& path_) :
    x(x_),
    y(y_),
    isRight(true),
    sprite(Game::instance().getResources().get(path_)),
    width(0),
    height(0),
    animationClip{0,0,0,0},
    boundingBox{(int)x_, (int)y_, 0, 0}
{
    // Only serves as the initializer for the derived classes.

    if(this->sprite != nullptr){
        this->width = this->sprite->getWidth();
        this->height = this->sprite->getHeight();
        
        this->boundingBox.w = this->width;
        this->boundingBox.h = this->height;
    }
    else{
        Log(WARN) << "GameObject sprite is null, width and height will be undefined.";
    }
}

GameObject::GameObject(const double x_, const double y_) :
    x(x_),
    y(y_),
    isRight(true),
    sprite(nullptr),
    width(0),
    height(0),
    animationClip{0,0,0,0},
    boundingBox{(int)x_, (int)y_, 0, 0}
{
    // Only serves as the initializer for the derived classes.
}

GameObject::~GameObject(){

}

unsigned int GameObject::getWidth(){
	return this->width;
}

unsigned int GameObject::getHeight(){
	return this->height;
}

SDL_Rect& GameObject::getAnimationClip(){
	return this->animationClip;
}

SDL_Rect& GameObject::getBoundingBox(){
    return this->boundingBox;
}
