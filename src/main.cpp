#include "Road.hpp"
#include "Window.hpp"
#include "MainMenu.hpp"
#include "Gorilla.hpp"
#include "Car.hpp"
#include "Success.hpp"

void check_car_collision(Road &road, Car &car, Window &win)
{
    static bool left_col = false;
    static bool right_col = false;

    if (road.getCollisionLeft(car.getSprite().getGlobalBounds())) {
        if (!left_col) {
            car.takeDamage();
            left_col = true;
        }
    } else if (car.getSprite().getPosition().x > 275)
        left_col = false;
    if (road.getCollisionRight(car.getSprite().getGlobalBounds())) {
        if (!right_col) {
            car.takeDamage();
            right_col = true;
        }
    } else if (car.getSprite().getPosition().x < 525)
        right_col = false;

    auto en = win.getEnemies();
    for (auto i: en) {
        if (i.getGlobalBounds().intersects(car.getSprite().getGlobalBounds())) {
            car.takeDamage();
            break;
        }
    }
    if (car.getState() == 0)
        return;
    std::string s;
    if (car.isGameOver())
        s = "Crashed the car";
    else
        s = "Got hit " + std::to_string(car.getState()) + " time";
    if (!car.isGameOver() && car.getState() > 1)
        s += 's';
    win.addSuccess(s);
}

void check_menu_event(Window &win, MainMenu &menu, sf::Event &ev, Road &r)
{
    if (ev.type == sf::Event::MouseButtonPressed) {
        if (menu.is_exit(ev) && win.stop == 0) {
            win.addSuccess("Tried to exit");
            if (win.nbSuccess() == 0)
                win.close();
        }
        if (menu.is_exit(ev) && win.stop == 1) {
            win.addSuccess("Back to menu");
            win.setMode(MAIN_MENU);
            r.setSpeed(5);
            win.stop = 0;
        } else if (menu.is_play(ev)) {
            win.addSuccess("Played the game");
            win.setMode(PLAY);
            win.stop = 0;
        }
    }
}

void poll_events(Window &win, MainMenu &menu, Road &r)
{
    sf::Event ev;

    while (win.pollEvent(ev)) {
        if (ev.type == sf::Event::Closed ||
        ((ev.type == sf::Event::KeyPressed && win.getMode() == MAIN_MENU) && ev.key.code == sf::Keyboard::Escape)) {
            win.addSuccess("Tried to exit");
            if (win.nbSuccess() == 0)
                win.close();
        }
        if (
        (ev.type == sf::Event::KeyPressed && ev.key.code == sf::Keyboard::Escape && win.getMode() != MAIN_MENU)) {
            win.addSuccess("Do you want a little break?");
            win.stop = !win.stop;
        }
        if (win.getMode() == MAIN_MENU || win.stop)
            check_menu_event(win, menu, ev, r);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) && sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        win.addSuccess("Drift god");
}

void check_success(Window &win)
{
    Success *cur = win.curSuccess();
    if (cur->msElapsed() < 2000) {
        if (!cur->hasPlayed())
            cur->play();
        cur->draw_to(win);
    } else {
        win.popSuccess();
        if (win.curSuccess())
            win.curSuccess()->reset();
    }
}

void move(Road &road, Car &car, Window &win)
{
    if (!road.getCollisionRight(car.getSprite().getGlobalBounds())) {
        car.move_right(win);
    } else
        win.addSuccess("Touched the right wall");
    if (!road.getCollisionLeft(car.getSprite().getGlobalBounds())) {
        car.move_left(win);
    } else
        win.addSuccess("Touched the left wall");
}

void draw_win(Window &win, MainMenu &menu, Road &road, Car &car, Gorilla &gorilla)
{
    win.clear(sf::Color::Black);
    if (win.getMode() == MAIN_MENU) {
        menu.draw_to(win);
    } else {
        road.draw(win);
        car.draw_to(win);
    }
    if (win.stop) {
        win.draw_dark();
        menu.draw_to(win);
    } else if (win.getMode() != MAIN_MENU)
        gorilla.draw(win);
    win.drawEnemies();
    if (win.nbSuccess() > 0)
        check_success(win);
    win.display();
}

void move_all(Window &win, Road &road, Car &car)
{
    if (win.getMode() != MAIN_MENU) {
        check_car_collision(road, car, win);
        move(road, car, win);
        road.move_back();
        win.moveEnemies(road);
    }
}

int main(void)
{
    Window win(sf::VideoMode(800, 600), "Route du succès", sf::Style::Close | sf::Style::Resize);
    MainMenu menu(sf::Vector2f(800, 600));
    Road road(win);
    Car car;
    Gorilla gorilla;

    win.addSuccess("Launched the game");
    win.addSuccess("First success");
    while (win.isOpen()) {
        if (win.stop == 0)
            move_all(win, road, car);
        poll_events(win, menu, road);
        draw_win(win, menu, road, car, gorilla);
    }
    return 0;
}
