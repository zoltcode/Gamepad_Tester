#include "GUI.h"
#include "controller_icon.h"

static const unsigned char* controller_map = assets_noun_xbox_controller_7424807_png;
static const unsigned int controller_map_len = assets_noun_xbox_controller_7424807_png_len;


static void helpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}



GUI::GUI(Window& window, GamepadManager& gamepadManager) : m_window(window), m_gamepad(gamepadManager)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    // Initialize Backends
    ImGui_ImplSDL3_InitForSDLRenderer(m_window.getWindow(), m_window.getRenderer());
    ImGui_ImplSDLRenderer3_Init(m_window.getRenderer());

    m_texture = TextureLoader::loadFromMemory(
        m_window.getRenderer(),
        controller_map,
        controller_map_len
    );
}


GUI::~GUI()
{
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}


void GUI::newFrame()
{
    ImGuiIO& io = ImGui::GetIO();

    // 1. Get window size and renderer info
    int w, h;
    int bw, bh;
    SDL_GetWindowSize(m_window.getWindow(), &w, &h);
    SDL_GetRenderOutputSize(m_window.getRenderer(), &bw, &bh);

    // 2. Prevent crash: manually set display size and scale if they are 0
    io.DisplaySize = ImVec2((float)w, (float)h);

    if (w > 0 && h > 0) {
        io.DisplayFramebufferScale = ImVec2((float)bw / w, (float)bh / h);
    } else {
        io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    }

    // 3. Check for the specific Viewport crash condition
    ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    if (main_viewport) {
        if (main_viewport->FramebufferScale.x <= 0) main_viewport->FramebufferScale.x = 1.0f;
        if (main_viewport->FramebufferScale.y <= 0) main_viewport->FramebufferScale.y = 1.0f;
    }

    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}


void GUI::updateUI()
{
    int w, h;
    SDL_GetWindowSize(m_window.getWindow(), &w, &h);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)w, (float)h));

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

    ImGui::Begin("MainUI", nullptr, flags);

    ImGui::Text("Gamepad Diagnostic Tool v" GAMEPAD_TESTER_VERSION);
    ImGui::Separator();

    if (m_gamepad.isConnected()) {
        // --- 1. Controller Visualization ---
        if (m_texture) {

            ImVec2 image_display_size(600, 600 * 0.85f);


            ImGui::Image(
                (ImTextureID)m_texture,
                         image_display_size,
                         ImVec2(0, 0),          // UV0 (Top Left)
            ImVec2(1, 0.85f),          // UV1 (Bottom Right)
            ImVec4(1, 1, 1, 1),    // Tint Color (White)
            ImVec4(0, 0, 0, 0)     // Border Color (Transparent/None)
            );

            // --- CLICK TRICK START ---
            if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                ImVec2 mousePos = ImGui::GetMousePos();      // Global screen position of the mouse
                ImVec2 anchor = ImGui::GetItemRectMin();     // Global screen position of the image top-left corner


                float relativeX = mousePos.x - anchor.x;
                float relativeY = mousePos.y - anchor.y;


                std::cout << "Click at: X=" << std::fixed << std::setprecision(1)
                << relativeX << ", Y=" << relativeY << std::endl;
            }
            // --- CLICK TRICK END ---

            // Get the bounding box of the image to use as a coordinate anchor
            ImVec2 anchor = ImGui::GetItemRectMin();
            ImDrawList* draw_list = ImGui::GetWindowDrawList();


            // --- TRIGGER BARS RENDERER ---
            auto drawTrigger = [&](SDL_GamepadAxis axis, float xOffset, const char* label) {
                float val = m_gamepad.getAxis(axis); // Current pressure (0.0 to 1.0)

                float barWidth = 20.0f;
                float barHeight = 200.0f;

                // Position: Right edge of the image + additional X offset
                ImVec2 barPosTopLeft = ImVec2(anchor.x + image_display_size.x + xOffset, anchor.y + 50);
                ImVec2 barPosBottomRight = ImVec2(barPosTopLeft.x + barWidth, barPosTopLeft.y + barHeight);


                draw_list->AddRectFilled(barPosTopLeft, barPosBottomRight, IM_COL32(40, 40, 40, 255));

                // Draw Filling (from top to bottom)
                // Calculating the bottom Y coordinate of the red area based on axis value
                float fillBottomY = barPosTopLeft.y + (barHeight * val);
                draw_list->AddRectFilled(barPosTopLeft, ImVec2(barPosBottomRight.x, fillBottomY), IM_COL32(255, 0, 0, 255));

                draw_list->AddRect(barPosTopLeft, barPosBottomRight, IM_COL32(200, 200, 200, 255));

                draw_list->AddText(ImVec2(barPosTopLeft.x, barPosTopLeft.y - 20), IM_COL32(255, 255, 255, 255), label);
            };

            // Draw both triggers to the right of the image
            drawTrigger(SDL_GAMEPAD_AXIS_LEFT_TRIGGER, 30.0f, "L2");
            drawTrigger(SDL_GAMEPAD_AXIS_RIGHT_TRIGGER, 70.0f, "R2");



            // Overlay circles on button presses
            auto drawBtn = [&](SDL_GamepadButton btn, float offsetX, float offsetY) {
                if (m_gamepad.isButtonPressed(btn)) {
                    draw_list->AddCircleFilled(ImVec2(anchor.x + offsetX, anchor.y + offsetY), 15.0f, IM_COL32(255, 0, 0, 200));
                }
            };


            drawBtn(SDL_GAMEPAD_BUTTON_SOUTH, 458, 180 + 67); // A
            drawBtn(SDL_GAMEPAD_BUTTON_EAST,  498, 140 + 67); // B
            drawBtn(SDL_GAMEPAD_BUTTON_WEST,  418, 140 + 67); // X
            drawBtn(SDL_GAMEPAD_BUTTON_NORTH, 458, 100 + 67); // Y

            // Back / Select Button (Usually on the left)
            if (m_gamepad.isButtonPressed(SDL_GAMEPAD_BUTTON_BACK)) {
                draw_list->AddCircleFilled(ImVec2(anchor.x + 263, anchor.y + 208), 10.0f, IM_COL32(255, 0, 0, 200));
            }

            // Start / Menu Button (Usually on the right)
            if (m_gamepad.isButtonPressed(SDL_GAMEPAD_BUTTON_START)) {
                draw_list->AddCircleFilled(ImVec2(anchor.x + 352, anchor.y + 208), 10.0f, IM_COL32(255, 0, 0, 200));
            }

            // Function to draw a circle for stick clicks (L3, R3)
            auto drawStickBtn = [&](SDL_GamepadButton btn, float offsetX, float offsetY) {
                if (m_gamepad.isButtonPressed(btn)) {
                    draw_list->AddCircleFilled(ImVec2(anchor.x + offsetX, anchor.y + offsetY), 30.0f, IM_COL32(255, 0, 0, 200));
                }
            };

            drawStickBtn(SDL_GAMEPAD_BUTTON_LEFT_STICK, 155, 208);
            drawStickBtn(SDL_GAMEPAD_BUTTON_RIGHT_STICK, 382, 298);

            // Function to draw a rectangle for D-pad directions
            auto drawDPad = [&](SDL_GamepadButton btn, float xOffset, float yOffset, float w, float h) {
                if (m_gamepad.isButtonPressed(btn)) {
                    ImVec2 posTopLeft = ImVec2(anchor.x + xOffset, anchor.y + yOffset);
                    ImVec2 posBottomRight = ImVec2(posTopLeft.x + w, posTopLeft.y + h);

                    draw_list->AddRectFilled(posTopLeft, posBottomRight, IM_COL32(255, 0, 0, 200));
                }
            };


            drawDPad(SDL_GAMEPAD_BUTTON_DPAD_UP,    217.0f, 264.0f, 24.0f, 28.0f);
            drawDPad(SDL_GAMEPAD_BUTTON_DPAD_DOWN,  217.0f, 321.0f, 24.0f, 28.0f);
            drawDPad(SDL_GAMEPAD_BUTTON_DPAD_LEFT,  178.0f, 294.0f, 28.0f, 24.0f);
            drawDPad(SDL_GAMEPAD_BUTTON_DPAD_RIGHT, 247.0f, 294.0f, 28.0f, 24.0f);


            // Helper function to draw custom shaped buttons (LB/RB)
            auto drawShapedBtn = [&](SDL_GamepadButton btn, const std::vector<ImVec2>& relativePoints) {
                if (m_gamepad.isButtonPressed(btn)) {
                    ImDrawList* draw_list = ImGui::GetWindowDrawList();

                    // Convert relative points to absolute screen positions
                    std::vector<ImVec2> absPoints;
                    for (const auto& p : relativePoints) {
                        absPoints.push_back(ImVec2(anchor.x + p.x, anchor.y + p.y));
                    }

                    // Draw the filled shape
                    draw_list->AddConvexPolyFilled(absPoints.data(), (int)absPoints.size(), IM_COL32(255, 0, 0, 200));
                }
            };

            // LB Points (Tracing the left shoulder button)
            std::vector<ImVec2> lbPoints = {
                {99, 124}, {162, 103}, {173, 109}, {96, 131}
            };
            drawShapedBtn(SDL_GAMEPAD_BUTTON_LEFT_SHOULDER, lbPoints);

            // RB Points (Tracing the right shoulder button)
            std::vector<ImVec2> rbPoints = {
                {503, 124}, {438, 103}, {426, 109}, {503, 131}
            };
            drawShapedBtn(SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER, rbPoints);


            ImGui::Spacing(); // Add some vertical space after the image
        }

        // --- 1. Display connected device info ---
        ImGui::Text("Connected Device: %s", m_gamepad.getGamepadName());
        ImGui::Spacing();

        // --- 2. Layout for Sticks and Rumble ---
        // This is a table with 2 columns.
        // Column 0: Stretches to fill available space (where your stick squares will go).
        // Column 1: Fixed at 200 pixels for the rumble controls.
        if (ImGui::BeginTable("LowerLayout", 2))
        {
            ImGui::TableSetupColumn("SticksArea", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("RumbleArea", ImGuiTableColumnFlags_WidthFixed, 200.0f);

            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            auto drawStickVisualizer = [&](const char* label, SDL_GamepadAxis axisX, SDL_GamepadAxis axisY) {
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                ImVec2 size(200, 200);
                ImVec2 pos = ImGui::GetCursorScreenPos();
                ImVec2 center = ImVec2(pos.x + size.x / 2.0f, pos.y + size.y / 2.0f);

                // 1. Draw the Square Border & Center Crosshair
                draw_list->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(150, 150, 150, 255), 0, 0, 1.5f);
                draw_list->AddLine(ImVec2(center.x, pos.y), ImVec2(center.x, pos.y + size.y), IM_COL32(80, 80, 80, 255));
                draw_list->AddLine(ImVec2(pos.x, center.y), ImVec2(pos.x + size.x, center.y), IM_COL32(80, 80, 80, 255));

                // 2. Circularity Guide
                draw_list->AddCircle(center, size.x / 2.0f, IM_COL32(0, 255, 0, 100), 64);

                // 3. Fetch Honest Data
                float x = m_gamepad.getAxis(axisX); // Normalized -1.0 to 1.0
                float y = m_gamepad.getAxis(axisY);

                // 4. Map to screen
                ImVec2 dotPos = ImVec2(center.x + x * (size.x / 2.0f), center.y + y * (size.y / 2.0f));

                // 5. Draw the Stick Position (Red for high visibility)
                draw_list->AddCircleFilled(dotPos, 6.0f, IM_COL32(255, 0, 0, 255));

                // 6. Labels and Values
                ImGui::Dummy(size); // Reserve space for the drawing
                ImGui::Text("%s", label);
                ImGui::Text("X: %.4f Y: %.4f", x, y);
            };

            // Position the sticks slightly lower than the text
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

            // Draw Left Stick
            ImGui::BeginGroup();
            drawStickVisualizer("Left Stick", SDL_GAMEPAD_AXIS_LEFTX, SDL_GAMEPAD_AXIS_LEFTY);
            ImGui::EndGroup();

            ImGui::SameLine(0, 20); // 40px gap between the two squares

            // Draw Right Stick
            ImGui::BeginGroup();
            drawStickVisualizer("Right Stick", SDL_GAMEPAD_AXIS_RIGHTX, SDL_GAMEPAD_AXIS_RIGHTY);
            ImGui::EndGroup();

            ImGui::TableNextColumn();

            ImGui::PushItemWidth(200.0f);


            ImGui::Text("Connection Performance");
            ImGui::Separator();

            const GamepadStats& stats = m_gamepad.getStats();

            // Logic: Use the moving average stats from your LatencyStats class
            // Green for 1000Hz+, Yellow for 125-500Hz, Red for < 125Hz
            ImVec4 hzColor = ImVec4(1, 0, 0, 1); // Default Red
            if (stats.polling_rate_hz >= 500) hzColor = ImVec4(0, 1, 0, 1);      // Green
            else if (stats.polling_rate_hz >= 240) hzColor = ImVec4(1, 1, 0, 1); // Yellow

            ImGui::Text("Polling Rate:");
            ImGui::SameLine();
            ImGui::TextColored(hzColor, "%.0f Hz", stats.polling_rate_hz);
            ImGui::SameLine();
            helpMarker("To measure accurately move the analog sticks around in a chaotic circular pattern as fast as you can. Controllers often change the frequency from maximum to minimum required to accurately send input because of power saving.");

            ImGui::Text("Avg Latency:");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "%.2f ms", stats.avg_latency_ms);
            ImGui::SameLine();
            helpMarker("Hardware-to-OS delay. Lower is better.");

            ImGui::Spacing();
            ImGui::Spacing(); // Add double spacing to separate from Rumble


            ImGui::Text("Rumble Settings");
            ImGui::Separator();

            // Labels are placed inside the sliders to save horizontal space
            ImGui::SliderFloat("##Weak", &m_weakValue, 0.0f, 1.0f, "Weak: %.2f");
            ImGui::SliderFloat("##Strong", &m_strongValue, 0.0f, 1.0f, "Strong: %.2f");

            // Button spans the full width of the 200px column
            if (ImGui::Button("Test Rumble", ImVec2(200, 40))) {
                m_gamepad.rumble(m_weakValue, m_strongValue, 1000);
            }

            ImGui::PopItemWidth();

            ImGui::EndTable();
        }
    } else {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "No Gamepad Connected");
    }

    ImGui::End();
}



void GUI::render()
{
    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), m_window.getRenderer());
}


