#include <utility>
#include <vector>
#include <map>
#include <array>

using coordinate_t = long long int;

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

class Position
{
    public:
    Position(std::pair<coordinate_t, coordinate_t> pos)
    {
        x = pos.first;
        y = pos.second;
    }
    coordinate_t getX() {
        return x;
    }
    coordinate_t getY() {
        return y;
    }
    private:
    coordinate_t x;
    coordinate_t y;
};

struct NotLanded : public std::exception {
    const char *what() const throw() { return "Rover has not landed yet"; }
};

class RoverState
{
    public:
    RoverState()
    {
        this->landed = false;
        this->stopped = false;
    }
    void land(std::pair<coordinate_t, coordinate_t> position, Direction direction)
    {
        this->position = std::make_shared<Position>(position);
        this->direction = direction;
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
    [[nodiscard]] bool getStopped() const {
        return stopped;
    }
    [[nodiscard]] Direction getDirection() const {
        return direction;
    }
    [[nodiscard]] std::shared_ptr<Position> getPosition() const {
        return position;
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
    
    Action(ActionType a)
    {
        type = a;
        actions = NULL;
    }
    Action(std::initializer_list<ActionType> actions)
    {
        actions = new std::vector<Action>(actions);
        type = NULL;
    }
    bool execute(Rover& rover) // true jak sie powiodło, false jak się zatrzymał
    {
        if (actions == NULL)
        {
            if (type == ActionType::move_forward)
            {
                Position pos = rover.state.get_forward_position();
                if (rover.is_danger())
                {
                    return false;
                }
                rover.state.move_forward();
            } 
            if (type == ActionType::move_backward)
            {
                Position pos = rover.state.get_backward_position();
                if (rover.is_danger())
                {
                    return false;
                }
                rover.state.move_backward();
            }
            if (type == ActionType::rotate_left)
            {
                rover.state.rotate_left();
            }
            if (type == ActionType::rotate_right)
            {
                rover.state.rotate_right();
            }  
        } 
        else
        {
            for (auto a: actions)
            {
                if (!a.execute())
                {
                    return false;
                }
            }
        }
        return true;
    }
    private:
    ActionType type; // tylko jeśli to pojedynczy typ
    std::vector<Action> actions; // tylko jeśli to compose
};

class MoveForward : Action {

};

class MoveBackward : Action {

};

class RotateLeft : Action {

};

class RotateRight : Action {

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

Action compose(std::initializer_list<Action> actions)
{
    return new Action(actions);
}

class Sensor
{
    virtual bool is_safe([[maybe_unused]] coordinate_t x,
                 [[maybe_unused]] coordinate_t y) {}
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
            if(!commands.contains(c))
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
    std::ostream& operator<<(std::ostream& os, const Rover& rover)
    {
        // os << "(" << num.l << ", " << num.m << ", " << num.u << ")";
        if (!rover.state->getLanded())
        {
            os << "unknown";
        }
        else
        {
            os << "(" << rover.state->getPosition()->getX() << ", " << rover.state->getPosition()->getY() << ")";
            if (rover.state->getDirection() == Direction::WEST)
            {
                os << " WEST";
            }
            Direction dir = rover.state->getDirection()
            if (rover.state->getDirection() == Direction::WEST)
            {
                os << " NORTH";
            }
            if (rover.state.direction == Direction::WEST)
            {
                os << " EAST";
            }
            if (rover.state.direction == Direction::WEST)
            {
                os << " SOUTH";
            }
            if (rover.state.stopped)
            {
                os << " stopped";
            }
            os << "\n";
        }
        return os;
    }
    void land(std::pair<coordinate_t, coordinate_t> pos, Direction dir)
    {
        state->land(pos, dir);
    }
    bool isDanger(Position position)
    {
        for (auto s: sensors)
        {
            if (!s.is_safe())
            {
                return true;
            }
        }
        return false;
    }
private:
    std::map<char, std::shared_ptr<Action>> commands;
    std::vector<std::unique_ptr<Sensor>> sensors;
    std::shared_ptr<RoverState> state;
    std::ostream& operator<<(std::ostream& os, const Rover& rover)
    {
        // os << "(" << num.l << ", " << num.m << ", " << num.u << ")";
        if (!rover.state->landed)
        {
            os << "unknown";
        }
        else
        {
            os << "(" << rover.state.position.x << ", " << rover.state.position.y << ")";
            if (rover.state.direction == Direction::WEST)
            {
                os << " WEST";
            }
            if (rover.state.direction == Direction::WEST)
            {
                os << " NORTH";
            }
            if (rover.state.direction == Direction::WEST)
            {
                os << " EAST";
            }
            if (rover.state.direction == Direction::WEST)
            {
                os << " SOUTH";
            }
            if (rover.state.stopped)
            {
                os << " stopped";
            }
            os << "\n";
        }
        return os;
};

class RoverBuilder
{
public:
    RoverBuilder() = default;
    RoverBuilder(const RoverBuilder&& other) = delete;
    RoverBuilder(RoverBuilder&& other) = delete;

    RoverBuilder &program_command(char c, std::shared_ptr<Action> command)
    {
        commands[c] = command;
        return *this;
    }
    RoverBuilder &add_sensor(std::unique_ptr<Sensor> sensor)
    {
        sensors.emplace_back(std::move(sensor));
        return *this;
    }
    Rover build()
    {
        return Rover(std::move(commands), std::move(sensors));
    }
private:
    std::map<char, std::shared_ptr<Action>> commands;
    std::vector<std::unique_ptr<Sensor>> sensors;
};




