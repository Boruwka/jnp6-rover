class Direction
{
};

class Position
{
};

class Rover()
{
    public:
    void execute()
    {
    }
    void operator<< ()
    {
    }
    void land()
    {
    }
    private:
    Position position;
};

class RoverBuilder()
{
    public:
    RoverBuilder()
    {
        
    }
    // na pewno program_command i add_sensor musi być albo na RoverBuilder do RoverBuilder, albo void na RoverBuilder
    void program_command(char command, Action action)
    {
       
    }
    void add_sensor(std::unique_ptr<Sensor> sensor)
    {
    }
    Rover build()
    {
    }
    private:
        std::map<char, Action> commands;
};

class Sensor()
{
    bool is_safe([[maybe_unused]] coordinate_t x,
                 [[maybe_unused]] coordinate_t y) 
    {
    }
};

class Action()
{
    
};

class RoverState()
{
};
