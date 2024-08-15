#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.hpp>
#include "geometry.h"
#include "triangular.h"
#include "rectangular.h"
#include <vector>
#include <atomic>

enum class DrawMode {
    Triangles,
    Rectangles
};

class Renderer {
public:
    Renderer(DrawMode initialMode = DrawMode::Triangles);
    ~Renderer();
    void run();
    void setScaleInterval(float interval); // Method to set the scale interval
    void setUnitsPerPixel(float units); // Method to set units per pixel
    void setPoints(const std::vector<Node>& points);
    void setTriangles(const std::vector<Triangle>& triangles);
    void setRectangles(const std::vector<Rectangle>& rectangles);
    void setDrawMode(DrawMode mode);

private:
    void handleEvents();
    void render();
    void setupGraphics();
    void drawGraphAxis();
    void drawPoints();
    void drawTriangles();
    void drawRectangles();
    void drawButtons();
    bool isMouseOverButton(const sf::RectangleShape& button);
    void zoom(float factor);

    float roundToNearestWholeNumber(float value);

    sf::RenderWindow window;
    std::atomic<bool> running;

    sf::Font font;
    std::vector<sf::Text> xAxisNumbers;
    std::vector<sf::Text> yAxisNumbers;
    std::vector<sf::RectangleShape> xAxisSubDivisions;
    std::vector<sf::RectangleShape> yAxisSubDivisions;
    std::vector<sf::RectangleShape> xAxisSubSubDivisions;
    std::vector<sf::RectangleShape> yAxisSubSubDivisions;

    std::vector<Node> points;
    std::vector<Triangle> triangles;
    std::vector<Rectangle> rectangles;

    sf::Text zoomInText;
    sf::Text zoomOutText;
    sf::RectangleShape zoomInButton; 
    sf::RectangleShape zoomOutButton;

    float scaleInterval = 1.0f; // Default scale interval
    float unitsPerPixel = 1.0f; // Default units per pixel
    const int subScaleInterval = 10; // Interval for smaller subdivisions

    sf::View view;
    bool panning = false;
    sf::Vector2i panStart;

    DrawMode currentDrawMode;
};

#endif // RENDERER_H
