#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include "Core/Board.hpp"
#include "Core/MoveGenerator.hpp"
#include "Core/BitOps.hpp"
#include "Core/AI.hpp"

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

    sf::RenderWindow window(sf::VideoMode({800, 800}), "C++ Local Chess Engine");
    window.setFramerateLimit(60);

    sf::Color lightSquare(238, 238, 210);
    sf::Color darkSquare(118, 150, 86);
    sf::Color highlightColor(255, 255, 51, 150);
    sf::Color validMoveDotColor(0, 0, 0, 50);
    sf::Color checkmateColor(220, 20, 60, 200);
    sf::Color checkColor(255, 140, 0, 200); 

    sf::Texture piecesTexture;
    if (!piecesTexture.loadFromFile("../../assets/pieces.png"))
    {
        std::cerr << "Error: Could not load pieces.png." << std::endl;
        return -1;
    }

    sf::Sprite pieceSprite(piecesTexture);

    const int pieceWidth = piecesTexture.getSize().x / 6;
    const int pieceHeight = piecesTexture.getSize().y / 2;

    const float scaleX = 100.0f / pieceWidth;
    const float scaleY = 100.0f / pieceHeight;
    pieceSprite.setScale({scaleX, scaleY});

    int textureColMap[6] = {5, 3, 2, 4, 1, 0};

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
                if (mousePressed->button == sf::Mouse::Button::Left && !isCheckmate && turn == Engine::WHITE)
                {
                    int file = mousePressed->position.x / 100;
                    int rank = 7 - (mousePressed->position.y / 100);
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
                                turn = Engine::BLACK;
                                currentMoves = Engine::MoveGenerator::generateLegalMoves(gameBoard, turn);
                                bool inCheck = Engine::MoveGenerator::isInCheck(gameBoard, turn);

                                if (currentMoves.empty())
                                {
                                    if (inCheck)
                                    {
                                        isCheckmate = true;
                                        checkmateSquare = Engine::getLSB(gameBoard.pieceBitboards[turn][Engine::KING]);
                                        std::cout << "Checkmate!" << std::endl;
                                    }
                                    else
                                    {
                                        std::cout << "Stalemate!" << std::endl;
                                    }
                                }
                                else
                                {
                                    if (inCheck)
                                    {
                                        isCheck = true;
                                        checkSquare = Engine::getLSB(gameBoard.pieceBitboards[turn][Engine::KING]);
                                        std::cout << "Check!" << std::endl;
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

        if (!isCheckmate && turn == Engine::BLACK)
        {
            window.display(); 

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
                    std::cout << "Checkmate!" << std::endl;
                }
                else
                {
                    std::cout << "Stalemate!" << std::endl;
                }
            }
            else
            {
                if (inCheck)
                {
                    isCheck = true;
                    checkSquare = Engine::getLSB(gameBoard.pieceBitboards[turn][Engine::KING]);
                    std::cout << "Check!" << std::endl;
                }
                else
                {
                    isCheck = false;
                }
            }
        }

        window.clear();

        for (int rank = 0; rank < 8; rank++)
        {
            for (int file = 0; file < 8; file++)
            {
                int sqIndex = rank * 8 + file;

                sf::RectangleShape square(sf::Vector2f(100.f, 100.f));
                square.setPosition(sf::Vector2f(file * 100.f, (7 - rank) * 100.f));

                if ((file + rank) % 2 == 0)
                {
                    square.setFillColor(darkSquare);
                }
                else
                {
                    square.setFillColor(lightSquare);
                }

                if (isCheckmate && sqIndex == checkmateSquare)
                {
                    square.setFillColor(checkmateColor);
                }
                else if (isCheck && sqIndex == checkSquare)
                {
                    square.setFillColor(checkColor);
                }
                else if (sqIndex == selectedSquare)
                {
                    square.setFillColor(highlightColor);
                }

                window.draw(square);

                if (selectedSquare != -1 && !isCheckmate)
                {
                    for (const auto &move : currentMoves)
                    {
                        if (move.getFrom() == selectedSquare && move.getTo() == sqIndex)
                        {
                            sf::CircleShape dot(15.f);
                            dot.setFillColor(validMoveDotColor);
                            dot.setPosition(sf::Vector2f(file * 100.f + 35.f, (7 - rank) * 100.f + 35.f));
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
                    pieceSprite.setPosition(sf::Vector2f(file * 100.f, (7 - rank) * 100.f));
                    window.draw(pieceSprite);
                }
            }
        }

        window.display();
    }

    return 0;
}