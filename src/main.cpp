#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include "Core/Board.hpp"
#include "Core/MoveGenerator.hpp"
#include "Core/BitOps.hpp"
#include "Core/AI.hpp"

enum GameState {
    MENU,
    PLAYING
};

int main()
{
    Engine::Board gameBoard;
    gameBoard.initStandardBoard();

    int turn = Engine::WHITE;
    int selectedSquare = -1;
    std::vector<Engine::Move> currentMoves = Engine::MoveGenerator::generateLegalMoves(gameBoard, turn);

    bool isCheckmate = false;
    int checkmateSquare = -1;
    bool isCheck = false;
    int checkSquare = -1;

    sf::Clock aiDelayClock;
    bool aiThinking = false;

    GameState currentState = MENU;
    bool isAIEnabled = false;

    const float SQ_SIZE = 150.f;

    sf::RenderWindow window(sf::VideoMode({1200, 1200}), "C++ Local Chess Engine");
    window.setFramerateLimit(60);

    sf::Color lightSquare(238, 238, 210);
    sf::Color darkSquare(118, 150, 86);
    sf::Color highlightColor(255, 255, 51, 150);
    sf::Color validMoveDotColor(0, 0, 0, 50);
    sf::Color checkmateColor(220, 20, 60, 200);
    sf::Color checkColor(255, 140, 0, 200);
    sf::Color lastMoveColor(255, 255, 0, 100);

    sf::Texture piecesTexture;
    if (!piecesTexture.loadFromFile("../../assets/pieces.png"))
    {
        std::cerr << "Error: Could not load pieces.png." << std::endl;
        return -1;
    }

    sf::Sprite pieceSprite(piecesTexture);

    const int pieceWidth = piecesTexture.getSize().x / 6;
    const int pieceHeight = piecesTexture.getSize().y / 2;

    const float scaleX = SQ_SIZE / pieceWidth;
    const float scaleY = SQ_SIZE / pieceHeight;
    pieceSprite.setScale({scaleX, scaleY});

    int textureColMap[6] = {5, 3, 2, 4, 1, 0};

    sf::Font font;
    if (!font.openFromFile("../../assets/ARIALBD.TTF"))
    {
        std::cerr << "Error: Could not load arial.ttf." << std::endl;
        return -1;
    }

    sf::RectangleShape pvpButton(sf::Vector2f(600.f, 100.f));
    pvpButton.setPosition(sf::Vector2f(300.f, 450.f));
    pvpButton.setFillColor(sf::Color(70, 70, 70));

    sf::Text pvpText(font);
    pvpText.setString("Player vs Player");
    pvpText.setCharacterSize(40);
    pvpText.setFillColor(sf::Color::White);
    pvpText.setPosition(sf::Vector2f(450.f, 475.f));

    sf::RectangleShape pveButton(sf::Vector2f(600.f, 100.f));
    pveButton.setPosition(sf::Vector2f(300.f, 600.f));
    pveButton.setFillColor(sf::Color(70, 70, 70));

    sf::Text pveText(font);
    pveText.setString("Player vs AI");
    pveText.setCharacterSize(40);
    pveText.setFillColor(sf::Color::White);
    pveText.setPosition(sf::Vector2f(490.f, 625.f));

    while (window.isOpen())
    {
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->code == sf::Keyboard::Key::Escape)
                {
                    window.close();
                }
            }

            if (const auto *mousePressed = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mousePressed->button == sf::Mouse::Button::Left)
                {
                    int mouseX = mousePressed->position.x;
                    int mouseY = mousePressed->position.y;

                    if (currentState == MENU)
                    {
                        if (mouseX >= 300 && mouseX <= 900 && mouseY >= 450 && mouseY <= 550)
                        {
                            isAIEnabled = false;
                            currentState = PLAYING;
                        }
                        else if (mouseX >= 300 && mouseX <= 900 && mouseY >= 600 && mouseY <= 700)
                        {
                            isAIEnabled = true;
                            currentState = PLAYING;
                        }
                    }
                    else if (currentState == PLAYING && !isCheckmate && (!isAIEnabled || turn == Engine::WHITE) && !aiThinking)
                    {
                        int file = mouseX / static_cast<int>(SQ_SIZE);
                        int rank = 7 - (mouseY / static_cast<int>(SQ_SIZE));
                        int clickedSquare = rank * 8 + file;

                        if (selectedSquare == -1)
                        {
                            int color, type;
                            if (gameBoard.getPieceAt(clickedSquare, color, type) && color == turn)
                            {
                                selectedSquare = clickedSquare;
                            }
                        }
                        else
                        {
                            bool moveFound = false;
                            for (const auto &move : currentMoves)
                            {
                                if (move.getFrom() == selectedSquare && move.getTo() == clickedSquare)
                                {
                                    gameBoard.makeMove(move, turn);
                                    turn ^= 1;
                                    currentMoves = Engine::MoveGenerator::generateLegalMoves(gameBoard, turn);
                                    bool inCheck = Engine::MoveGenerator::isInCheck(gameBoard, turn);

                                    if (currentMoves.empty())
                                    {
                                        if (inCheck)
                                        {
                                            isCheckmate = true;
                                            checkmateSquare = Engine::getLSB(gameBoard.pieceBitboards[turn][Engine::KING]);
                                        }
                                    }
                                    else
                                    {
                                        if (inCheck)
                                        {
                                            isCheck = true;
                                            checkSquare = Engine::getLSB(gameBoard.pieceBitboards[turn][Engine::KING]);
                                        }
                                        else
                                        {
                                            isCheck = false;
                                        }
                                    }

                                    moveFound = true;
                                    break;
                                }
                            }

                            if (!moveFound)
                            {
                                int color, type;
                                if (gameBoard.getPieceAt(clickedSquare, color, type) && color == turn)
                                {
                                    selectedSquare = clickedSquare;
                                }
                                else
                                {
                                    selectedSquare = -1;
                                }
                            }
                            else
                            {
                                selectedSquare = -1;
                            }
                        }
                    }
                }
            }
        }

        if (currentState == PLAYING)
        {
            if (isAIEnabled && !isCheckmate && turn == Engine::BLACK && !aiThinking)
            {
                aiThinking = true;
                aiDelayClock.restart();
            }

            if (aiThinking && aiDelayClock.getElapsedTime().asSeconds() > 0.6f)
            {
                Engine::Move aiMove = Engine::AI::getBestMove(gameBoard, 3, Engine::BLACK);
                gameBoard.makeMove(aiMove, turn);
                turn = Engine::WHITE;
                
                currentMoves = Engine::MoveGenerator::generateLegalMoves(gameBoard, turn);
                bool inCheck = Engine::MoveGenerator::isInCheck(gameBoard, turn);

                if (currentMoves.empty())
                {
                    if (inCheck)
                    {
                        isCheckmate = true;
                        checkmateSquare = Engine::getLSB(gameBoard.pieceBitboards[turn][Engine::KING]);
                    }
                }
                else
                {
                    if (inCheck)
                    {
                        isCheck = true;
                        checkSquare = Engine::getLSB(gameBoard.pieceBitboards[turn][Engine::KING]);
                    }
                    else
                    {
                        isCheck = false;
                    }
                }
                aiThinking = false;
            }
        }

        window.clear(sf::Color(40, 44, 52));

        if (currentState == MENU)
        {
            window.draw(pvpButton);
            window.draw(pvpText);
            window.draw(pveButton);
            window.draw(pveText);
        }
        else if (currentState == PLAYING)
        {
            for (int rank = 0; rank < 8; rank++)
            {
                for (int file = 0; file < 8; file++)
                {
                    int sqIndex = rank * 8 + file;

                    sf::RectangleShape square(sf::Vector2f(SQ_SIZE, SQ_SIZE));
                    square.setPosition(sf::Vector2f(file * SQ_SIZE, (7 - rank) * SQ_SIZE));

                    if ((file + rank) % 2 == 0)
                    {
                        square.setFillColor(darkSquare);
                    }
                    else
                    {
                        square.setFillColor(lightSquare);
                    }
                    window.draw(square);

                    float highlightRadius = 60.f;
                    float highlightOffset = (SQ_SIZE - (highlightRadius * 2.f)) / 2.f;

                    if (isCheckmate && sqIndex == checkmateSquare)
                    {
                        sf::CircleShape highlightCircle(highlightRadius);
                        highlightCircle.setFillColor(checkmateColor);
                        highlightCircle.setPosition(sf::Vector2f(file * SQ_SIZE + highlightOffset, (7 - rank) * SQ_SIZE + highlightOffset));
                        window.draw(highlightCircle);
                    }
                    else if (isCheck && sqIndex == checkSquare)
                    {
                        sf::CircleShape highlightCircle(highlightRadius);
                        highlightCircle.setFillColor(checkColor);
                        highlightCircle.setPosition(sf::Vector2f(file * SQ_SIZE + highlightOffset, (7 - rank) * SQ_SIZE + highlightOffset));
                        window.draw(highlightCircle);
                    }
                    else if (sqIndex == selectedSquare)
                    {
                        sf::CircleShape highlightCircle(highlightRadius);
                        highlightCircle.setFillColor(highlightColor);
                        highlightCircle.setPosition(sf::Vector2f(file * SQ_SIZE + highlightOffset, (7 - rank) * SQ_SIZE + highlightOffset));
                        window.draw(highlightCircle);
                    }
                    else if (gameBoard.hasLastMove && (sqIndex == gameBoard.lastMove.getFrom() || sqIndex == gameBoard.lastMove.getTo()))
                    {
                        sf::CircleShape highlightCircle(highlightRadius);
                        highlightCircle.setFillColor(lastMoveColor);
                        highlightCircle.setPosition(sf::Vector2f(file * SQ_SIZE + highlightOffset, (7 - rank) * SQ_SIZE + highlightOffset));
                        window.draw(highlightCircle);
                    }

                    if (selectedSquare != -1 && !isCheckmate && (!isAIEnabled || turn == Engine::WHITE))
                    {
                        for (const auto &move : currentMoves)
                        {
                            if (move.getFrom() == selectedSquare && move.getTo() == sqIndex)
                            {
                                float dotRadius = 20.f;
                                float dotOffset = (SQ_SIZE - (dotRadius * 2.f)) / 2.f;
                                sf::CircleShape dot(dotRadius);
                                dot.setFillColor(validMoveDotColor);
                                dot.setPosition(sf::Vector2f(file * SQ_SIZE + dotOffset, (7 - rank) * SQ_SIZE + dotOffset));
                                window.draw(dot);
                                break;
                            }
                        }
                    }

                    int color, type;
                    if (gameBoard.getPieceAt(sqIndex, color, type))
                    {
                        int texCol = textureColMap[type];
                        int texRow = color;

                        pieceSprite.setTextureRect(sf::IntRect({texCol * pieceWidth, texRow * pieceHeight}, {pieceWidth, pieceHeight}));
                        pieceSprite.setPosition(sf::Vector2f(file * SQ_SIZE, (7 - rank) * SQ_SIZE));
                        window.draw(pieceSprite);
                    }
                }
            }
        }

        window.display();
    }

    return 0;
}