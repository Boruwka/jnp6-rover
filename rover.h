#include <utility>
#include <vector>
#include <map>
#include <array>

using coordinate_t = long long int;

class RoverState;
class Action;
class Position;
class Position;

struct NotLanded : public std::exception {
    const char *what() const throw() { return "Rover has not landed yet"; }
};

enum class Direction
{
    WEST,
    EAST,
    SOUTH,
    NORTH
};

enum class ActionType
{
    move_forward,
    move_backward,
    rotate_left,
    rotate_right
};

class Sensor
{
public:
    virtual bool is_safe([[maybe_unused]] coordinate_t x,
                         [[maybe_unused]] coordinate_t y) {return true;}
};

class Action;

class Position
{
public:
    Position(std::pair<coordinate_t, coordinate_t> pos)
    {
        x = pos.first;
        y = pos.second;
    }
    [[nodiscard]] coordinate_t getX() const {
        return x;
    }
    void decrementX() {
        --x;
    }
    void incrementX() {
        ++x;
    }
    void decrementY() {
        --y;
    }
    void incrementY() {
        ++y;
    }
    [[nodiscard]] coordinate_t getY() const {
        return y;
    }
private:
    coordinate_t x;
    coordinate_t y;
};


class RoverState
{
public:
    RoverState()
    {
        this->landed = false;
        this->stopped = false;
    }
    void land(std::pair<coordinate_t, coordinate_t> _position, Direction _direction)
    {
        this->position = std::make_shared<Position>(_position);
        this->direction = _direction;
        this->landed = true;
        this->stopped = false;
    }
    [[nodiscard]] bool getLanded() const {
        return landed;
    }
    void setLanded(bool l) {
        this->landed = l;
    }
    void setStopped(bool s) {
        this->stopped = s;
    }
    [[nodiscard]] bool isStopped() const {
        return stopped;
    }
    [[nodiscard]] Direction getDirection() const {
        return direction;
    }
    [[nodiscard]] std::shared_ptr<Position> getPosition() const {
        return position;
    }
    std::shared_ptr<Position> get_forward_position()
    {
        switch (direction)
        {
            case (Direction::WEST):
                return std::make_shared<Position>(std::make_pair((position->getX() - 1), position->getY()));
            case (Direction::EAST):
                return std::make_shared<Position>(std::make_pair((position->getX() + 1), position->getY()));
            case (Direction::NORTH):
                return std::make_shared<Position>(std::make_pair((position->getX()), (position->getY() + 1)));
            default:
                return std::make_shared<Position>(std::make_pair((position->getX()), (position->getY() - 1)));
        }
    }
    std::shared_ptr<Position> get_backward_position()
    {
        switch (direction)
        {
            case (Direction::EAST):
                return std::make_shared<Position>(std::make_pair((position->getX() - 1), position->getY()));
            case (Direction::WEST):
                return std::make_shared<Position>(std::make_pair((position->getX() + 1), position->getY()));
            case (Direction::SOUTH):
                return std::make_shared<Position>(std::make_pair((position->getX()), (position->getY() + 1)));
            default:
                return std::make_shared<Position>(std::make_pair((position->getX()), (position->getY() - 1)));
        }
    }
    void move_forward()
    {
        switch (direction)
        {
            case (Direction::WEST):
                position->decrementX();
                break;
            case (Direction::EAST):
                position->incrementX();
                break;
            case (Direction::NORTH):
                position->incrementY();
                break;
            default:
                position->decrementY();
        }
    }
    void move_backward()
    {
        switch (direction)
        {
            case (Direction::WEST):
                position->incrementX();
                break;
            case (Direction::EAST):
                position->decrementX();
                break;
            case (Direction::NORTH):
                position->decrementY();
                break;
            default:
                position->incrementY();
        }
    }
    void rotate_left()
    {
        switch (direction)
        {
            case (Direction::WEST):
                direction = Direction::SOUTH;
                break;
            case (Direction::EAST):
                direction = Direction::NORTH;
                break;
            case (Direction::NORTH):
                direction = Direction::WEST;
                break;
            default:
                direction = Direction::EAST;
        }
    }
    void rotate_right()
    {
        switch(direction)
        {
            case(Direction::WEST):
                direction = Direction::NORTH;
                break;
            case(Direction::EAST):
                direction = Direction::SOUTH;
                break;
            case(Direction::NORTH):
                direction = Direction::EAST;
                break;
            default:
                direction = Direction::WEST;
        }
    }
private:
    bool landed;
    bool stopped;
    std::shared_ptr<Position> position;
    Direction direction;
};

class Rover;

class Action
{
public:

    virtual ~Action() {}

    virtual bool execute([[maybe_unused]] Rover &rover) const {
        return false;
    }
};


class Rover
{
public:
    Rover(std::map<char, std::shared_ptr<Action>> &&commands, std::vector<std::unique_ptr<Sensor>> &&sensors)
    {
        this->sensors = sensors;
        this->commands = commands;
        this->state = std::make_shared<RoverState>();
    }
    void execute(const std::string &command_string)
    {
        if (!state->getLanded())
        {
            throw NotLanded();
        }
        for (auto c: command_string)
        {
            if (!commands.contains(c))
            {
                state->setStopped(true);
                return;
            }
            if (!commands[c]->execute(*this))
            {
                state->setStopped(true);
            }
        }
    }
    void land(std::pair<coordinate_t, coordinate_t> pos, Direction dir)
    {
        state->land(pos, dir);
    }
    bool is_danger(const std::shared_ptr<Position> &position)
    {
        for (const auto &s : sensors)
        {
            if (!s->is_safe(position->getX(), position->getY()))
            {
                return true;
            }
        }
        return false;
    }
    RoverState &getState() const {
        return *state;
    }
    std::shared_ptr<RoverState> get_state()
    {
        return state;
    }
    friend std::ostream& operator<<(std::ostream& os, const Rover& rover);
private:
    std::map<char, std::shared_ptr<Action>> commands;
    std::vector<std::unique_ptr<Sensor>> sensors;
    std::shared_ptr<RoverState> state;
};

std::ostream& operator<<(std::ostream& os, const Rover& rover)
{
    // os << "(" << num.l << ", " << num.m << ", " << num.u << ")";
    if (!rover.getState().getLanded())
    {
        os << "unknown";
    }
    else
    {
        os << "(" << rover.getState().getPosition()->getX() << ", " << rover.getState().getPosition()->getY() << ")";
        switch (rover.getState().getDirection()) {
            case Direction::WEST:
                os << " WEST";
                break;
            case Direction::NORTH:
                os << " NORTH";
                break;
            case Direction::EAST:
                os << " EAST";
                break;
            default:
                os << " SOUTH";
                break;
        }
        if (rover.getState().isStopped())
        {
            os << " stopped";
        }
        os << "\n";
    }
    return os;
}



class MoveForward : public Action {
public:
    bool execute(Rover& rover) const override
    {
        if (rover.is_danger(rover.getState().get_forward_position()))
        {
            return false;
        }
        rover.getState().move_forward();
        return true;
    }
};

class MoveBackward : public Action {
public:
    bool execute(Rover& rover) const override
    {
        if (rover.is_danger(rover.getState().get_backward_position()))
        {
            return false;
        }
        rover.getState().move_backward();
        return true;
    }
};

class RotateLeft : public Action {
public:
    bool execute(Rover& rover) const override
    {
        rover.getState().rotate_left();
        return true;
    } 
};

class RotateRight : public Action {
public:
    bool execute(Rover& rover) const override
    {
        rover.getState().rotate_right();
        return true;
    } 
};

class Compose : public Action {
public:
    Compose(std::initializer_list<std::shared_ptr<Action>> _components) : components(_components) {}

    bool execute(Rover& rover) const override
    {
        for (const auto& a : components)
        {
            if (!a->execute(rover))
            {
                return false;
            }
        }
        return true;
    }
private:
    std::vector<std::shared_ptr<Action>> components;
};

std::shared_ptr<MoveForward> move_forward()
{
    return std::make_shared<MoveForward>();
}

std::shared_ptr<MoveBackward> move_backward()
{
    return std::make_shared<MoveBackward>();
}

std::shared_ptr<RotateLeft> rotate_left()
{
    return std::make_shared<RotateLeft>();
}

std::shared_ptr<RotateRight> rotate_right()
{
    return std::make_shared<RotateRight>();
}

std::shared_ptr<Action> compose(std::initializer_list<std::shared_ptr<Action>> actions)
{
    return std::make_shared<Compose>(actions);
}

class RoverBuilder
{
public:
    RoverBuilder() = default;
    RoverBuilder(const RoverBuilder&& other) = delete;
    RoverBuilder(RoverBuilder&& other) = delete;

    RoverBuilder &program_command(char c, std::shared_ptr<Action> command)
    {
        commands[c] = std::move(command);
        return *this;
    }
    RoverBuilder &add_sensor(std::unique_ptr<Sensor> sensor)
    {
        sensors.emplace_back(std::move(sensor));
        return *this;
    }
    Rover build()
    {
        return {std::move(commands), std::move(sensors)};
    }
private:
    std::map<char, std::shared_ptr<Action>> commands;
    std::vector<std::unique_ptr<Sensor>> sensors;
};
