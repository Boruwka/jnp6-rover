#include <vector>
#include <unordered_map>
#include <array>

using coordinate_t = double; // tego chyba nie powinno być ale idk

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
    private:
    coordinate_t x;
    coordinate_t y;
};



struct NotLanded : public std::exception {
    const char *what() const throw() { return "Rover has not landed yet"; }
};

struct UnknownCommand : public std::exception {
    const char *what() const throw() { return "Unknown command"; }
};

class RoverState
{
    public:
    RoverState(Position pos, Direction dir)
    {
        this->pos = pos;
        this->dir = dir;
    }
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
    void execute(Rover& rover)
    {
        if (actions == NULL)
        {
            if (type == ActionType::move_forward)
            {
                Position pos = rover.state.get_forward_position();
                if (rover.is_danger())
                {
                    throw DangerDetected;
                }
                rover.state.move_forward();
            } 
            if (type == ActionType::move_backward)
            {
                Position pos = rover.state.get_backward_position();
                if (rover.is_danger())
                {
                    throw DangerDetected;
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
                a.execute();
            }
        }
    }
    private:
    ActionType type; // tylko jeśli to pojedynczy typ
    std::vector<Action> actions; // tylko jeśli to compose
    
};

Action move_forward()
{
    return new Action(ActionType::move_forward);
}

Action move_backward()
{
    return new Action(ActionType::move_backward);
}

Action rotate_left()
{
    return new Action(ActionType::rotate_left);
}

Action rotate_right()
{
    return new Action(ActionType::rotate_right);
}

Action compose(std::initializer_list<Action> actions)
{
    return new Action(actions);
}


class Rover
{
    public:
    Rover(std::map<char, Action> commands, std::set<std::unique_ptr<Sensor>> sensors)
    {
        this.sensors = sensors;
        this.commands = commands;
        landed = false;
    }
    void execute(std::string command_string)
    {
        if (!landed)
        {
            throw NotLanded;
        }
        for (auto c: command_string) 
        {
            commands[c].execute(*this);
        }
    }
    void operator<< ()
    {
        // idk co to dokładnie ma robić
    }
    void land(std::pair<coordinate_t, coordinate_t> pos, Direction dir)
    {
        Position position = new Position(pos);
        state = new State(position, dir);
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
        bool landed;
        std::map<char, Action> commands;
        std::vector<std::unique_ptr<Sensor>> sensors;
};

class Sensor
{
    bool is_safe([[maybe_unused]] coordinate_t x,
                 [[maybe_unused]] coordinate_t y) {}
};

class RoverBuilder
{
    public:
    RoverBuilder() {}
    RoverBuilder &program_command(char c, Command &&command)
    {
        commands[c] = command;
        return *this;
    } 
    RoverBuilder &add_sensor(std::unique_ptr<Sensor> sensor)
    {
        sensors.insert(sensor);
        return *this;
    }
    Rover build()
    {
        return new Rover(commands, sensors);
    }
    private:
        std::unordered_map<char, Action> commands;
        std::vector<std::unique_ptr<Sensor>> sensors;
};




