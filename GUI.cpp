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
    io.IniFilename = nullptr;

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
    ImVec2 image_display_size(600, 600 * 0.75f);
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

            // --- CLICK TRICK START ---
            // if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            //     ImVec2 mousePos = ImGui::GetMousePos();      // Global screen position of the mouse
            //     ImVec2 anchor = ImGui::GetItemRectMin();     // Global screen position of the image top-left corner
            //
            //
            //     float relativeX = mousePos.x - anchor.x;
            //     float relativeY = mousePos.y - anchor.y;
            //
            //
            //     std::cout << "Click at: X=" << std::fixed << std::setprecision(1)
            //     << relativeX << ", Y=" << relativeY << std::endl;
            // }
            // --- CLICK TRICK END ---

        // --- 1. Display connected device info at the top ---
        ImGui::Text("Connected Device: %s", m_gamepad.getGamepadName());
        ImGui::Separator();

        // --- 2. Main Layout Table ---
        if (ImGui::BeginTable("MainLayout", 2, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_BordersInnerV))
        {
            // Column 0: Controller Outline & Triggers
            ImGui::TableSetupColumn("VisualizerColumn", ImGuiTableColumnFlags_WidthFixed, 600.0f);
            // Column 1: Statistics and Stick Squares
            ImGui::TableSetupColumn("DataColumn", ImGuiTableColumnFlags_WidthStretch);

            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            // ImVec2 anchor = ImGui::GetCursorScreenPos();
            // Get the bounding box of the image to use as a coordinate anchor
            ImVec2 anchor = ImGui::GetItemRectMin();
            ImDrawList* draw_list = ImGui::GetWindowDrawList();



            // Function to draw a rectangle for D-pad directions
            auto drawDPad = [&](SDL_GamepadButton btn, float xOffset, float yOffset, float w, float h) {
                if (m_gamepad.isButtonPressed(btn)) {
                    // TODO: properly update the points. This is a quick fix.
                    ImVec2 posTopLeft = ImVec2(anchor.x + xOffset, anchor.y + yOffset - 50);
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
                    // TODO: properly update the points. This is a quick fix
                    for (const auto& p : relativePoints) {
                        absPoints.push_back(ImVec2(anchor.x + p.x, anchor.y + p.y - 50));
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


            // --- DRAW CONTROLLER OUTLINE HERE ---
            if (m_texture) {
                ImGui::Image(
                    (ImTextureID)m_texture,
                             image_display_size,
                             ImVec2(0, 0.1f),          // UV0 (Top Left)
                ImVec2(1, 0.85f),          // UV1 (Bottom Right)
                ImVec4(1, 1, 1, 1),    // Tint Color (White)
                ImVec4(0, 0, 0, 0)     // Border Color (Transparent/None)
                );

                std::vector<ImVec2> lbPoints = {{99, 124}, {162, 103}, {173, 109}, {96, 131}};
                drawShapedBtn(SDL_GAMEPAD_BUTTON_LEFT_SHOULDER, lbPoints);

                std::vector<ImVec2> rbPoints = {{503, 124}, {438, 103}, {426, 109}, {503, 131}};
                drawShapedBtn(SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER, rbPoints);

                for (const auto& element : CONTROLLER_MAP) {
                    if (m_gamepad.isButtonPressed(element.button)) {
                        draw_list->AddCircleFilled(
                            ImVec2(anchor.x + element.x, anchor.y + element.y),
                                                   element.radius,
                                                   IM_COL32(255, 0, 0, 200) // Red with some transparency
                        );
                    }
                }
                // NEW CLICK TRICK
                // if (ImGui::IsItemClicked()) {
                //     ImVec2 mousePos = ImGui::GetMousePos();
                //     // Calculate relative coordinates from the top-left of the image
                //     float relativeX = mousePos.x - anchor.x;
                //     float relativeY = mousePos.y - anchor.y;
                //
                //     // Output to terminal/console
                //     printf("Clicked at: { %f, %f }\n", relativeX, relativeY);
                // }
            }

            // (Ensure 'anchor' is defined relative to the current cursor position)

            // [Insert your AddImage or AddLine calls for the controller outline here]

            ImGui::Spacing();
            ImGui::Separator();

            ImGui::Dummy(ImVec2(0.0f, 15.0f));
            // --- 3. Stick Visualizers (Side-by-Side) ---
            auto drawStickVisualizer = [&](const char* label, SDL_GamepadAxis axisX, SDL_GamepadAxis axisY) {
                ImGui::BeginGroup();
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                ImVec2 size(160, 160);
                ImVec2 pos = ImGui::GetCursorScreenPos();
                ImVec2 center = ImVec2(pos.x + size.x / 2.0f, pos.y + size.y / 2.0f);

                draw_list->AddRect(pos, ImVec2(pos.x + size.x, pos.y + size.y), IM_COL32(150, 150, 150, 255), 0, 0, 1.5f);
                draw_list->AddLine(ImVec2(center.x, pos.y), ImVec2(center.x, pos.y + size.y), IM_COL32(80, 80, 80, 255));
                draw_list->AddLine(ImVec2(pos.x, center.y), ImVec2(pos.x + size.x, center.y), IM_COL32(80, 80, 80, 255));
                draw_list->AddCircle(center, size.x / 2.0f, IM_COL32(0, 255, 0, 100), 64);

                float x = m_gamepad.getAxis(axisX);
                float y = m_gamepad.getAxis(axisY);
                ImVec2 dotPos = ImVec2(center.x + x * (size.x / 2.0f), center.y + y * (size.y / 2.0f));
                draw_list->AddCircleFilled(dotPos, 5.0f, IM_COL32(255, 0, 0, 255));

                ImGui::Dummy(size);
                ImGui::Text("%s: X:%.2f Y:%.2f", label, x, y);
                ImGui::EndGroup();
            };

            ImGui::BeginGroup();
            if (ImGui::BeginTable("StickGrid", 2, ImGuiTableFlags_None, ImVec2(600, 0)))
            {
                ImGui::TableSetupColumn("L", ImGuiTableColumnFlags_WidthFixed, 300.0f);
                ImGui::TableSetupColumn("R", ImGuiTableColumnFlags_WidthFixed, 300.0f);

                ImGui::TableNextColumn();
                // Center the 160px box inside the 300px column: (300 - 160) / 2 = 70
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 70.0f);
                drawStickVisualizer("Left", SDL_GAMEPAD_AXIS_LEFTX, SDL_GAMEPAD_AXIS_LEFTY);

                ImGui::TableNextColumn();
                // Do the same for the right column
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 70.0f);
                drawStickVisualizer("Right", SDL_GAMEPAD_AXIS_RIGHTX, SDL_GAMEPAD_AXIS_RIGHTY);

                ImGui::EndTable();
            }

            ImGui::EndGroup();

            // ImGui::Dummy(ImVec2(600, 20));


            // --- Switch to Right Column for Data ---
            ImGui::TableNextColumn();
            // --- TRIGGER BARS RENDERER ---
            auto drawTrigger = [&](SDL_GamepadAxis axis, float xOffset, const char* label) {
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
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
                ImGui::Dummy(ImVec2(barWidth, barHeight));
            };

            // Draw both triggers to the right of the image
            ImGui::BeginGroup();
            drawTrigger(SDL_GAMEPAD_AXIS_LEFT_TRIGGER, 30.0f, "L2");
            drawTrigger(SDL_GAMEPAD_AXIS_RIGHT_TRIGGER, 70.0f, "R2");
            ImGui::EndGroup();


            // --- 4. Performance Metrics ---
            ImGui::SameLine(150.0f);
            ImGui::BeginGroup();
            ImGui::Dummy(ImVec2(0, 50)); // Align with the top of the trigger bars
            ImGui::Text("Performance Metrics");
            const GamepadStats& stats = m_gamepad.getStats();
            ImVec4 hzColor = (stats.polling_rate_hz >= 500) ? ImVec4(0, 1, 0, 1) : (stats.polling_rate_hz >= 240) ? ImVec4(1, 1, 0, 1) : ImVec4(1, 0, 0, 1);

            ImGui::Text("Polling rate:"); ImGui::SameLine();
            ImGui::TextColored(hzColor, "%.0f Hz", stats.polling_rate_hz);
            ImGui::SameLine();
            helpMarker("How many times per second the controller sends data to the PC.\n\n"
            "Higher values (e.g., 1000Hz) mean smoother movement and lower input delay. "
            "To measure accurately rapidly move your sticks back and forth. "
            "\nModern controllers have a power-saving feature that lowers the polling rate according to what's needed to accurately depict the movement.");

            ImGui::Text("Latency: "); ImGui::SameLine();
            ImGui::TextColored(ImVec4(0, 0.8f, 1, 1), "%.2f ms", stats.avg_latency_ms);
            ImGui::SameLine();
            helpMarker("The estimated time it takes for an input to be processed by the software.\n\n"
            "Lower is better. This value depends on the polling rate and the system's processing speed.");

            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

            // --- 5. Rumble Testing ---
            ImGui::Text("Haptics / Rumble");
            float itemWidth = ImGui::GetContentRegionAvail().x - 5.0f;
            ImGui::PushItemWidth(itemWidth);
            ImGui::SliderFloat("##Weak", &m_weakValue, 0.0f, 1.0f, "Weak: %.2f");
            ImGui::SliderFloat("##Strong", &m_strongValue, 0.0f, 1.0f, "Strong: %.2f");
            if (ImGui::Button("Test Rumble", ImVec2(itemWidth, 40))) {
                m_gamepad.rumble(m_weakValue, m_strongValue, 1000);
            }
            ImGui::PopItemWidth();
            ImGui::EndGroup();



            // --- 6. Touchpad Visualizer ---
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Text("Touchpad Tracking");

            // Define drawing area
            ImVec2 tpSize(400.0f, 180.0f);
            // Center the touchpad in the column (assuming ~600px available based on your StickGrid)
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetContentRegionAvail().x - tpSize.x) * 0.5f);

            ImVec2 tpPos = ImGui::GetCursorScreenPos();
            // ImDrawList* draw_list = ImGui::GetWindowDrawList();

            // Draw Background and Outline
            draw_list->AddRectFilled(tpPos, ImVec2(tpPos.x + tpSize.x, tpPos.y + tpSize.y), IM_COL32(30, 30, 30, 255));
            draw_list->AddRect(tpPos, ImVec2(tpPos.x + tpSize.x, tpPos.y + tpSize.y), IM_COL32(255, 255, 255, 255), 0.0f, 0, 1.5f);

            // Fetch and draw fingers
            // Most gamepads only have 1 touchpad (index 0) with 2 fingers
            int numFingers = m_gamepad.getNumTouchpadFingers(0);
            for (int i = 0; i < numFingers; i++) {
                TouchpadFinger finger = m_gamepad.getTouchpadFinger(0, i);

                if (finger.down) {
                    // Map normalized SDL coordinates (0.0-1.0) to UI rect
                    ImVec2 fingerPos = ImVec2(tpPos.x + (finger.x * tpSize.x),
                                              tpPos.y + (finger.y * tpSize.y));

                    int alpha = (int)(finger.pressure * 255.0f);
                    if (alpha < 60) alpha = 60;

                    draw_list->AddCircleFilled(fingerPos, 8.0f, IM_COL32(255, 0, 0, alpha));
                    draw_list->AddCircle(fingerPos, 8.0f, IM_COL32(255, 255, 255, 150), 0, 1.0f);
                }
            }

            ImGui::Dummy(tpSize);




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


