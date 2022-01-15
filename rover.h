#include <vector>
#include <map>
#include <array>

using coordinate_t = long long int;

enum class DirectionType
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
    void land(Position pos, Direction dir)
    {
        this->pos = pos;
        this->dir = dir;
        this->landed = true;
        this->stopped = false;
    }
    bool landed;
    bool stopped;
    private:
    Position pos;
    Direction dir;
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
    Rover(std::map<char, Action> commands, std::set<std::unique_ptr<Sensor>> sensors)
    {
        this.sensors = sensors;
        this.commands = commands;
        this.state = new RoverState();
    }
    void execute(std::string command_string)
    {
        if (!state.landed)
        {
            throw NotLanded;
        }
        for (auto c: command_string)
        {
            if(!commands.contains(c))
            {
                state.stopped = true;
                return;
            }
            if (!commands[c].execute(*this))
            {
                state.stopped = true;
            }
        }
    }
    std::ostream& operator<<(std::ostream& os, const Rover& rover)
    {
        // os << "(" << num.l << ", " << num.m << ", " << num.u << ")";
        if (!rover.state.landed)
        {
            os << "unknown";
        }
        else
        {
            os << "(" << rover.state.position.x << ", " << rover.state.position.y << ")";
            if (rover.state.direction == DirectionType::WEST)
            {
                os << " WEST";
            }
            if (rover.state.direction == DirectionType::WEST)
            {
                os << " NORTH";
            }
            if (rover.state.direction == DirectionType::WEST)
            {
                os << " EAST";
            }
            if (rover.state.direction == DirectionType::WEST)
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
        Position position = new Position(pos);
        state.land(position, dir);
    }
    bool is_danger(Position position)
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
    RoverState state;
    std::ostream& operator<<(std::ostream& os, const Rover& rover)
    {
        // os << "(" << num.l << ", " << num.m << ", " << num.u << ")";
        if (!rover.state.landed)
        {
            os << "unknown";
        }
        else
        {
            os << "(" << rover.state.position.x << ", " << rover.state.position.y << ")";
            if (rover.state.direction == DirectionType::WEST)
            {
                os << " WEST";
            }
            if (rover.state.direction == DirectionType::WEST)
            {
                os << " NORTH";
            }
            if (rover.state.direction == DirectionType::WEST)
            {
                os << " EAST";
            }
            if (rover.state.direction == DirectionType::WEST)
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
    std::map<char, Action> commands;
    std::vector<std::unique_ptr<Sensor>> sensors;
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




