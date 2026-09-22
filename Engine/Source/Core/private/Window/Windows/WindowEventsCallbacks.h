#pragma once

#include <Camera.h>
#include <Constants.h>
#include <Timer.h>
#include <Window/IWindow.h>
#include <Window.h>

#include "INIReader.h"

namespace GameEngine::Core
{
    static const float kMinTimeBetweenPresses = 0.1;

    void OnMouseDown(WPARAM btnState, int x, int y, Window* window)
    {
        window->SetMousePos(x, y);

        SetCapture(GetPlatformWindowHandle(window->GetWindowHandle()));
    }

    void OnMouseUp(WPARAM btnState, int x, int y)
    {
        ReleaseCapture();
    }

    void OnMouseMove(WPARAM btnState, int x, int y, Camera* camera, Window* window)
    {
        Math::Vector2i mousePos = window->GetMousePos();
        if ((btnState & MK_LBUTTON) != 0)
        {
            float dx = 0.25 * static_cast<float>(x - mousePos.x) * Math::Constants::PI / 180.f;
            float dy = 0.25 * static_cast<float>(y - mousePos.y) * Math::Constants::PI / 180.f;

            dy = -dy; // To avoid inverse movement

            camera->Rotate(dx, dy);
        }
        else if ((btnState & MK_RBUTTON) != 0)
        {
            float dx = 0.05f * static_cast<float>(x - mousePos.x);
            float dy = 0.05f * static_cast<float>(y - mousePos.y);

            Math::Vector3f offset = camera->GetViewDir() * (dx - dy);

            Math::Vector3f position = camera->GetPosition();
            position = position + offset;

            camera->SetPosition(position);
        }

        window->SetMousePos(x, y);
    }

    void OnKeyDown(WPARAM wParam, Camera* camera)
    {
        std::string path = "../../../../../Configs/camera.ini";

        static INIReader reader(path);
        assert(reader.ParseError() >= 0);

        Math::Vector3f offset = Math::Vector3f::Zero();
        Math::Vector3f position = camera->GetPosition();

        static Timer timer;
        float dt;

        //static int Forward(reader.GetInteger("Camera", "Forward", 'W'));
        //static int Backward(reader.GetInteger("Camera", "Backward", 'S'));
        //static int Left(reader.GetInteger("Camera", "Left", 'A'));
        //static int Right(reader.GetInteger("Camera", "Right", 'D'));
        //static int Reset(reader.GetInteger("Camera", "Reset", 'R'));

        static int Forward = reader.Get("Camera", "Forward", "W")[0];
        static int Backward = reader.Get("Camera", "Backward", "S")[0];
        static int Left = reader.Get("Camera", "Left", "A")[0];
        static int Right = reader.Get("Camera", "Right", "D")[0];
        static int Reset = reader.Get("Camera", "Reset", "R")[0];

        if (wParam == Forward)
        {
            timer.Tick();
            dt = timer.GetDeltaTime() < kMinTimeBetweenPresses ? timer.GetDeltaTime() : kMinTimeBetweenPresses;

            offset += camera->GetForwardDir() * dt;
            //offset = camera->GetViewDir() * 1.0;
        }
        else if (wParam == Backward)
        {
            timer.Tick();
            dt = timer.GetDeltaTime() < kMinTimeBetweenPresses ? timer.GetDeltaTime() : kMinTimeBetweenPresses;

            offset += -camera->GetForwardDir() * dt;
        }
        else if (wParam == Left)
        {
            timer.Tick();
            dt = timer.GetDeltaTime() < kMinTimeBetweenPresses ? timer.GetDeltaTime() : kMinTimeBetweenPresses;

            offset += -camera->GetRightDir() * dt;
        }
        else if (wParam == Right)
        {
            timer.Tick();
            dt = timer.GetDeltaTime() < kMinTimeBetweenPresses ? timer.GetDeltaTime() : kMinTimeBetweenPresses;

            offset += camera->GetRightDir() * dt;
        }
        else if (wParam == Reset)
        {
            camera->Reset();
            return;
        }
        else
        {
            return;
        }

        position = position + offset;
        camera->SetPosition(position);
    }

    void Check() {
        constexpr int g = std::string("A")[0];
        constexpr int b = g;
        static_assert(g == 'A');
    }
}