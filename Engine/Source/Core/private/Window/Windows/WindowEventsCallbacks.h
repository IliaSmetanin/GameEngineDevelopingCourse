#pragma once

#include <Camera.h>
#include <Constants.h>
#include <Window/IWindow.h>
#include <Window.h>

#include "INIReader.h"

namespace GameEngine::Core
{
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

    void OnKeyDown(WPARAM wParam, Camera* camera) {
        // После раскомментирования строчки ниже, все падает с ошибкой линковки
        // static INIReader reader("camera.ini");

        using STCLOCK_t = std::chrono::steady_clock::time_point;

        Math::Vector3f offset;
        Math::Vector3f position = camera->GetPosition();

        STCLOCK_t last = std::chrono::steady_clock::now();

        switch (wParam)
        {
        case 'A':
            while (GetAsyncKeyState(wParam) < 0) {
                STCLOCK_t current = std::chrono::steady_clock::now();
                std::chrono::duration<float, std::ratio<1, 1>> frame = current - last;
                last = current;

                float dt = frame.count();

                offset = Math::Vector3f(-(camera->GetViewDir().z), 0.0, camera->GetViewDir().x).Normalized();

                position = position + offset * dt;

                camera->SetPosition(position);
            }

            return;
        case 'D':
            offset = Math::Vector3f(camera->GetViewDir().z, 0.0, -(camera->GetViewDir().x));
            break;
        case 'W':
            offset = camera->GetViewDir() * 1.0;
            break;
        case 'S':
            offset = camera->GetViewDir() * -1.0;
            break;
        case 'R':
            camera->Reset();
            return;
        default:
            return;
        }

        position = position + offset;
        camera->SetPosition(position);
    }
}