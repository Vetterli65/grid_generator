#include "renderer.h"
#include <iostream>
#include <cmath>

Renderer::Renderer(DrawMode initialMode)
    : window(sf::VideoMode(800, 600), "Mesh Visualizer", sf::Style::Default, sf::ContextSettings(0, 0, 8)),
    running(true), currentDrawMode(initialMode) {
    window.setVerticalSyncEnabled(true);
    if (!font.loadFromFile("../Fonts/Arial.ttf")) {
        std::cerr << "Failed to load font from ../Fonts/Arial.ttf" << std::endl;
        running = false;
    }
    else {
        view.setSize(window.getDefaultView().getSize());
        view.setCenter(0.f, 0.f);
        window.setView(view);
        setupGraphics();

        // Initialize buttons with fixed sizes
        sf::Vector2u windowSize = window.getSize();
        float buttonWidth = windowSize.x * 0.05f; 
        float buttonHeight = windowSize.y * 0.05f; 
        float padding = 10.f;

        zoomInButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
        zoomInButton.setFillColor(sf::Color::Green);
        zoomInButton.setPosition(windowSize.x - buttonWidth - padding, padding);

        zoomOutButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
        zoomOutButton.setFillColor(sf::Color::Red);
        zoomOutButton.setPosition(windowSize.x - buttonWidth - padding, padding + buttonHeight + padding);

        // Initialize button text
        zoomInText.setFont(font);
        zoomInText.setString("Z+");
        zoomInText.setCharacterSize(buttonHeight * 0.5f);
        zoomInText.setFillColor(sf::Color::Black);
        zoomInText.setOrigin(zoomInText.getLocalBounds().width / 2, zoomInText.getLocalBounds().height / 2);
        zoomInText.setPosition(zoomInButton.getPosition().x + buttonWidth / 2, zoomInButton.getPosition().y + buttonHeight / 2);

        zoomOutText.setFont(font);
        zoomOutText.setString("Z-");
        zoomOutText.setCharacterSize(buttonHeight * 0.5f);
        zoomOutText.setFillColor(sf::Color::Black);
        zoomOutText.setOrigin(zoomOutText.getLocalBounds().width / 2, zoomOutText.getLocalBounds().height / 2);
        zoomOutText.setPosition(zoomOutButton.getPosition().x + buttonWidth / 2, zoomOutButton.getPosition().y + buttonHeight / 2);

    }
}

Renderer::~Renderer() {}

void Renderer::run() {
    while (running) {
        handleEvents();
        render();
        sf::sleep(sf::milliseconds(10));
    }
}

void Renderer::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            running = false;
            window.close();
        }
        else if (event.type == sf::Event::Resized) {
            window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
            view = window.getView();
            view.setCenter(0.f, 0.f);
            window.setView(view);
            setupGraphics();
        }
        else if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                if (isMouseOverButton(zoomInButton)) {
                    zoom(1.1f); 
                }
                else if (isMouseOverButton(zoomOutButton)) {
                    zoom(0.9f); 
                }
                else {
                    panning = true;
                    panStart = sf::Mouse::getPosition(window);
                }
            }
        }
        else if (event.type == sf::Event::MouseButtonReleased) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                panning = false;
            }
        }
        else if (event.type == sf::Event::MouseMoved) {
            if (panning) {
                sf::Vector2i panEnd = sf::Mouse::getPosition(window);
                sf::Vector2f offset = window.mapPixelToCoords(panStart) - window.mapPixelToCoords(panEnd);
                view.move(offset);
                window.setView(view);
                panStart = panEnd;
                setupGraphics();
            }
        }
        else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::T) {
                setDrawMode(DrawMode::Triangles);
            }
            else if (event.key.code == sf::Keyboard::R) {
                setDrawMode(DrawMode::Rectangles);
            }
        }
    }
}

void Renderer::render() {
    window.clear();
    window.setView(view);

    drawGraphAxis();
    if (currentDrawMode == DrawMode::Triangles) {
        drawTriangles();
    }
    else if (currentDrawMode == DrawMode::Rectangles) {
        drawRectangles();
    }
    drawPoints();
    drawButtons();

    window.display();
}

void Renderer::setupGraphics() {
    xAxisNumbers.clear();
    yAxisNumbers.clear();
    xAxisSubDivisions.clear();
    yAxisSubDivisions.clear();
    xAxisSubSubDivisions.clear();
    yAxisSubSubDivisions.clear();

    drawGraphAxis();
}

void Renderer::drawGraphAxis() {
    // Draw major axis lines
    sf::RectangleShape xAxis(sf::Vector2f(view.getSize().x, 2.f));
    sf::RectangleShape yAxis(sf::Vector2f(2.f, view.getSize().y));

    xAxis.setOrigin(xAxis.getSize().x / 2.f, xAxis.getSize().y / 2.f);
    xAxis.setPosition(view.getCenter().x, 0.f);

    yAxis.setOrigin(yAxis.getSize().x / 2.f, yAxis.getSize().y / 2.f);
    yAxis.setPosition(0.f, view.getCenter().y);

    window.draw(xAxis);
    window.draw(yAxis);

    xAxisNumbers.clear();
    yAxisNumbers.clear();
    xAxisSubDivisions.clear();
    yAxisSubDivisions.clear();
    xAxisSubSubDivisions.clear();
    yAxisSubSubDivisions.clear();

    // Drawing subdivion and subsubdivision
    sf::Vector2f subDivSize(2.f, 10.f);
    sf::Vector2f subSubDivSize(1.f, 5.f); // Smaller subdivision size

    sf::FloatRect viewBounds(view.getCenter() - view.getSize() / 2.f, view.getSize());

    // Adjust based on current view and units per pixel
    float left = viewBounds.left * unitsPerPixel;
    float right = (viewBounds.left + viewBounds.width) * unitsPerPixel;
    float top = viewBounds.top * unitsPerPixel;
    float bottom = (viewBounds.top + viewBounds.height) * unitsPerPixel;

    // Create x-axis numbers and divisions
    for (int i = std::ceil(left / scaleInterval); i <= std::floor(right / scaleInterval); ++i) {\
        float xPos = i * scaleInterval / unitsPerPixel;
        sf::Text number;
        number.setFont(font);
        number.setString(std::to_string(i * static_cast<int>(scaleInterval)));
        number.setCharacterSize(12);
        number.setFillColor(sf::Color::White);
        sf::FloatRect bounds = number.getLocalBounds();
        number.setOrigin(bounds.width / 2, 0);
        number.setPosition(xPos, 5.f);
        xAxisNumbers.push_back(number);

        sf::RectangleShape subDiv(sf::Vector2f(subDivSize.x, subDivSize.y));
        subDiv.setFillColor(sf::Color::White);
        subDiv.setPosition(xPos - subDivSize.x / 2, -subDivSize.y / 2);
        xAxisSubDivisions.push_back(subDiv);

        // Smaller subdivisions
        float subSubScaleInterval = scaleInterval / 10.0f;
        for (int j = 1; j < 10; ++j) {
            float subSubDivPos = (i * scaleInterval + j * subSubScaleInterval) / unitsPerPixel;
            sf::RectangleShape subSubDiv(sf::Vector2f(subSubDivSize.x, subSubDivSize.y));
            subSubDiv.setFillColor(sf::Color::White);
            subSubDiv.setPosition(subSubDivPos - subSubDivSize.x / 2, -subSubDivSize.y / 2);
            xAxisSubSubDivisions.push_back(subSubDiv);
        }
    }

    // Create y-axis numbers and divisions
    for (int i = std::ceil(top / scaleInterval); i <= std::floor(bottom / scaleInterval); ++i) {
        float yPos = i * scaleInterval / unitsPerPixel;
        sf::Text number;
        number.setFont(font);
        number.setString(std::to_string(-i * static_cast<int>(scaleInterval)));
        number.setCharacterSize(12);
        number.setFillColor(sf::Color::White);
        sf::FloatRect bounds = number.getLocalBounds();
        number.setOrigin(bounds.width / 2, bounds.height / 2);
        number.setPosition(15.f, yPos - bounds.height / 2);  
        yAxisNumbers.push_back(number);

        sf::RectangleShape subDiv(sf::Vector2f(subDivSize.y, subDivSize.x));
        subDiv.setFillColor(sf::Color::White);
        subDiv.setPosition(-subDivSize.y / 2, yPos - subDivSize.x / 2);
        yAxisSubDivisions.push_back(subDiv);

        // Smaller subdivisions
        float subSubScaleInterval = scaleInterval / 10.0f; 
        for (int j = 1; j < 10; ++j) {
            float subSubDivPos = (i * scaleInterval + j * subSubScaleInterval) / unitsPerPixel;
            sf::RectangleShape subSubDiv(sf::Vector2f(subSubDivSize.y, subSubDivSize.x));
            subSubDiv.setFillColor(sf::Color::White);
            subSubDiv.setPosition(-subSubDivSize.y / 2, subSubDivPos - subSubDivSize.x / 2);
            yAxisSubSubDivisions.push_back(subSubDiv);
        }
    }

    for (const auto& number : xAxisNumbers) {
        window.draw(number);
    }

    for (const auto& number : yAxisNumbers) {
        window.draw(number);
    }

    for (const auto& subDiv : xAxisSubDivisions) {
        window.draw(subDiv);
    }

    for (const auto& subDiv : yAxisSubDivisions) {
        window.draw(subDiv);
    }

    for (const auto& subSubDiv : xAxisSubSubDivisions) {
        window.draw(subSubDiv);
    }

    for (const auto& subSubDiv : yAxisSubSubDivisions) {
        window.draw(subSubDiv);
    }
}

void Renderer::drawButtons() {
    sf::Vector2u windowSize = window.getSize();

    window.setView(window.getDefaultView());
    window.draw(zoomInButton);
    window.draw(zoomOutButton);
    window.draw(zoomInText);
    window.draw(zoomOutText);
    window.setView(view); 
}


bool Renderer::isMouseOverButton(const sf::RectangleShape& button) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::FloatRect bounds = button.getGlobalBounds();
    return bounds.contains(static_cast<sf::Vector2f>(mousePos));
}

void Renderer::zoom(float factor) {
    view.zoom(factor);
    window.setView(view);
    setupGraphics();
}

void Renderer::drawPoints() {
    for (const auto& point : points) {
        sf::CircleShape shape(2); 
        float xPos = point.x / unitsPerPixel - 2;
        float yPos = -point.y / unitsPerPixel - 2;
        shape.setPosition(sf::Vector2f(xPos, yPos));
        shape.setOutlineColor(sf::Color(102, 102, 102));
        shape.setOutlineThickness(1.5);
        shape.setFillColor(sf::Color::White);
        window.draw(shape);
    }
}

void Renderer::drawTriangles() {
    for (const auto& triangle : triangles) {
        sf::ConvexShape shape;
        shape.setPointCount(3);
        for (int i = 0; i < 3; ++i) {
            float xPos = points[triangle.vertices[i]].x / unitsPerPixel;
            float yPos = -points[triangle.vertices[i]].y / unitsPerPixel;
            shape.setPoint(i, sf::Vector2f(xPos, yPos));
        }
        shape.setFillColor(sf::Color(255, 255, 255, 128)); 
        shape.setOutlineColor(sf::Color::White);
        shape.setOutlineThickness(1);
        window.draw(shape);
    }
}

void Renderer::drawRectangles() {
    for (const auto& rectangle : rectangles) {
        sf::ConvexShape shape;
        shape.setPointCount(4);
        for (int i = 0; i < 4; ++i) {
            float xPos = points[rectangle.vertices[i]].x / unitsPerPixel;
            float yPos = -points[rectangle.vertices[i]].y / unitsPerPixel;
            shape.setPoint(i, sf::Vector2f(xPos, yPos));
        }
        shape.setFillColor(sf::Color(255, 255, 255, 128)); 
        shape.setOutlineColor(sf::Color::White);
        shape.setOutlineThickness(1);
        window.draw(shape);
    }
}

void Renderer::setScaleInterval(float interval) {
    scaleInterval = roundToNearestWholeNumber(interval);
    setupGraphics(); 
}

void Renderer::setUnitsPerPixel(float units) {
    unitsPerPixel = units;
    setupGraphics(); 
}

void Renderer::setPoints(const std::vector<Node>& points) {
    this->points = points;
    setupGraphics(); 
}

void Renderer::setTriangles(const std::vector<Triangle>& triangles) {
    this->triangles = triangles;
    setupGraphics(); 
}

void Renderer::setRectangles(const std::vector<Rectangle>& rectangles) {
    this->rectangles = rectangles;
    setupGraphics(); 
}

void Renderer::setDrawMode(DrawMode mode) {
    currentDrawMode = mode;
}

float Renderer::roundToNearestWholeNumber(float value) {
    return std::round(value);
}
