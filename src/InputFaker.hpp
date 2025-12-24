#pragma once


enum GameCommand;

class InputFaker
{
public:
    static void SendKeyDownAndUp(const std::string& keyname);
    static void SendKey(const std::string& keyname, bool down);
    static void SendMouseMotion(int xrel, int yrel);
    static int SendCommand(GameCommand command_id, bool down);

    static inline int64_t* game_input_manager;

private:
    static void SendKeyMouse(const std::string& keyname, bool down);
    static void SendKeyKeyboard(const std::string& keyname, bool down, const std::vector<std::string>& modifiers);
};
