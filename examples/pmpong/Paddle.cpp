// SPDX-License-Identifier: BSD-3-Clause
/* Copyright 2017-2018, Intel Corporation */

#include "Paddle.hpp"
#include "Pool.hpp"

Paddle::Paddle(int x, int y)
{
	this->x = x;
	this->y = y;
	this->points = 0;
	init();
}

Paddle::~Paddle()
{
}

void
Paddle::moveUp(int velocity)
{
	if (!(this->y - velocity <
	      SCORE_VIEW_OFFSET + HORIZONAL_LINE_OFFSET + LINE_THICKNESS)) {
		setY(this->y - velocity);
	} else if (this->y - velocity <
		   SCORE_VIEW_OFFSET + HORIZONAL_LINE_OFFSET + LINE_THICKNESS) {
		setY(SCORE_VIEW_OFFSET + HORIZONAL_LINE_OFFSET +
		     LINE_THICKNESS);
	}
}

void
Paddle::moveDown(int velocity)
{
	if (!(this->y + PADDLE_HEIGHT + velocity >
	      WINDOW_HEIGHT - HORIZONAL_LINE_OFFSET - LINE_THICKNESS)) {
		setY(this->y + velocity);
	} else if (this->y + PADDLE_HEIGHT + velocity >
		   WINDOW_HEIGHT - HORIZONAL_LINE_OFFSET - LINE_THICKNESS) {
		setY(WINDOW_HEIGHT - HORIZONAL_LINE_OFFSET - PADDLE_HEIGHT);
	}
}

void
Paddle::addPoint()
{
	setPoints(points + 1);
}

void
Paddle::init()
{
	setY(WINDOW_HEIGHT / 2 - (int)getPaddleShape().getSize().y / 2);
}

void
Paddle::adjustPaddleYtoBall(Ball &ball)
{
	if (this->y > ball.getY())
		moveUp(PADDLE_VELOCITY_COMPUTER);
	if (this->y + getPaddleShape().getGlobalBounds().height -
		    ball.getBallShape().getRadius() * 4 <
	    ball.getY())
		moveDown(PADDLE_VELOCITY_COMPUTER);
}

void
Paddle::collisionWithBall(Ball &ball, bool increaseBallSpeed)
{
	if (ball.getBallShape().getGlobalBounds().intersects(
		    getPaddleShape().getGlobalBounds())) {
		ball.setVelocityX(ball.getVelocity()->x * (-1));
		if (increaseBallSpeed)
			ball.increaseVelocity();
	}
}

int
Paddle::getX()
{
	return this->x;
}

int
Paddle::getY()
{
	return this->y;
}

int
Paddle::getPoints()
{
	return this->points;
}

sf::RectangleShape
Paddle::getPaddleShape()
{
	sf::RectangleShape shapeToRet;
	shapeToRet.setSize(sf::Vector2f(PADDLE_WIDTH, PADDLE_HEIGHT));
	shapeToRet.setPosition(sf::Vector2f((float)this->x, (float)this->y));
	return shapeToRet;
}

void
Paddle::setPoints(int pointsArg)
{
	pmem::obj::transaction::run(Pool::getGamePool()->getPoolToTransaction(),
				    [&] { points = pointsArg; });
}

void
Paddle::setY(int yArg)
{
	pmem::obj::transaction::run(Pool::getGamePool()->getPoolToTransaction(),
				    [&] { y = yArg; });
}

void
Paddle::setX(int xArg)
{
	pmem::obj::transaction::run(Pool::getGamePool()->getPoolToTransaction(),
				    [&] { x = xArg; });
}
