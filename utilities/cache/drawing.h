#include <sstream>
class drawing_v
{
public:
	void Draw_Text(const std::string& text, const ImVec2& pos, float size, uint32_t color, bool center) {
		constexpr float StrokeValue = 1.0f;
		uint32_t EdgeColor = 0xFF000000;

		float EdgeA = (EdgeColor >> 24) & 0xff;
		float EdgeR = (EdgeColor >> 16) & 0xff;
		float EdgeG = (EdgeColor >> 8) & 0xff;
		float EdgeB = (EdgeColor) & 0xff;

		std::stringstream StringStream(text);
		std::string Line;
		float Y = 0.0f;
		int I = 0;

		while (std::getline(StringStream, Line)) {
			ImVec2 TextSize = ImGui::GetFont()->CalcTextSizeA(size, FLT_MAX, 0.0f, Line.c_str());

			if (center) {
					ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, ImVec2((pos.x - TextSize.x / 2.0f) - StrokeValue, pos.y + TextSize.y * I), ImGui::GetColorU32(ImVec4(EdgeR / 255, EdgeG / 255, EdgeB / 255, EdgeA / 255)), Line.c_str());
					ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, ImVec2((pos.x - TextSize.x / 2.0f) + StrokeValue, pos.y + TextSize.y * I), ImGui::GetColorU32(ImVec4(EdgeR / 255, EdgeG / 255, EdgeB / 255, EdgeA / 255)), Line.c_str());
					ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, ImVec2((pos.x - TextSize.x / 2.0f), (pos.y + TextSize.y * I) - StrokeValue), ImGui::GetColorU32(ImVec4(EdgeR / 255, EdgeG / 255, EdgeB / 255, EdgeA / 255)), Line.c_str());
					ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, ImVec2((pos.x - TextSize.x / 2.0f), (pos.y + TextSize.y * I) + StrokeValue), ImGui::GetColorU32(ImVec4(EdgeR / 255, EdgeG / 255, EdgeB / 255, EdgeA / 255)), Line.c_str());
				
				ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, ImVec2(pos.x - TextSize.x / 2.0f, pos.y + TextSize.y * I), color, Line.c_str());
			}
			else {
					ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, ImVec2((pos.x) - StrokeValue, (pos.y + TextSize.y * I)), ImGui::GetColorU32(ImVec4(EdgeR / 255, EdgeG / 255, EdgeB / 255, EdgeA / 255)), Line.c_str());
					ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, ImVec2((pos.x) + StrokeValue, (pos.y + TextSize.y * I)), ImGui::GetColorU32(ImVec4(EdgeR / 255, EdgeG / 255, EdgeB / 255, EdgeA / 255)), Line.c_str());
					ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, ImVec2((pos.x), (pos.y + TextSize.y * I) - StrokeValue), ImGui::GetColorU32(ImVec4(EdgeR / 255, EdgeG / 255, EdgeB / 255, EdgeA / 255)), Line.c_str());
					ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, ImVec2((pos.x), (pos.y + TextSize.y * I) + StrokeValue), ImGui::GetColorU32(ImVec4(EdgeR / 255, EdgeG / 255, EdgeB / 255, EdgeA / 255)), Line.c_str());
				
				ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, ImVec2(pos.x, pos.y + TextSize.y * I), color, Line.c_str());
			}

			Y = pos.y + TextSize.y * (I + 1);
			I++;
		}
	}
private:
	ImFont font{ };
}; drawing_v drawing;
using namespace std;
void text(const string& text, const ImVec2& pos, float size, ImColor color, bool center) {
	constexpr float StrokeValue = 0.9f;
	uint32_t EdgeColor = 0xFF000000;

	float EdgeA = (EdgeColor >> 24) & 0xff;
	float EdgeR = (EdgeColor >> 16) & 0xff;
	float EdgeG = (EdgeColor >> 8) & 0xff;
	float EdgeB = (EdgeColor) & 0xff;

	stringstream StringStream(text);
	string Line;
	float Y = 0.0f;
	int I = 0;
	ImGui::PushFont(menus.MenuFont);
	while (getline(StringStream, Line)) {
		ImVec2 TextSize = ImGui::GetFont()->CalcTextSizeA(size, FLT_MAX, 0.0f, Line.c_str());

		if (center) {
			if (visuals.outlinetext) {
				ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, ImVec2((pos.x - TextSize.x / 2.0f) - StrokeValue, pos.y + TextSize.y * I), ImGui::GetColorU32(ImVec4(EdgeR / 255, EdgeG / 255, EdgeB / 255, EdgeA / 255)), Line.c_str());
				ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, ImVec2((pos.x - TextSize.x / 2.0f) + StrokeValue, pos.y + TextSize.y * I), ImGui::GetColorU32(ImVec4(EdgeR / 255, EdgeG / 255, EdgeB / 255, EdgeA / 255)), Line.c_str());
				ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, ImVec2((pos.x - TextSize.x / 2.0f), (pos.y + TextSize.y * I) - StrokeValue), ImGui::GetColorU32(ImVec4(EdgeR / 255, EdgeG / 255, EdgeB / 255, EdgeA / 255)), Line.c_str());
				ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, ImVec2((pos.x - TextSize.x / 2.0f), (pos.y + TextSize.y * I) + StrokeValue), ImGui::GetColorU32(ImVec4(EdgeR / 255, EdgeG / 255, EdgeB / 255, EdgeA / 255)), Line.c_str());
			}
			ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, ImVec2(pos.x - TextSize.x / 2.0f, pos.y + TextSize.y * I), color, Line.c_str());
		}
		else {
			if (visuals.outlinetext) {
				ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, ImVec2((pos.x) - StrokeValue, (pos.y + TextSize.y * I)), ImGui::GetColorU32(ImVec4(EdgeR / 255, EdgeG / 255, EdgeB / 255, EdgeA / 255)), Line.c_str());
				ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, ImVec2((pos.x) + StrokeValue, (pos.y + TextSize.y * I)), ImGui::GetColorU32(ImVec4(EdgeR / 255, EdgeG / 255, EdgeB / 255, EdgeA / 255)), Line.c_str());
				ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, ImVec2((pos.x), (pos.y + TextSize.y * I) - StrokeValue), ImGui::GetColorU32(ImVec4(EdgeR / 255, EdgeG / 255, EdgeB / 255, EdgeA / 255)), Line.c_str());
				ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, ImVec2((pos.x), (pos.y + TextSize.y * I) + StrokeValue), ImGui::GetColorU32(ImVec4(EdgeR / 255, EdgeG / 255, EdgeB / 255, EdgeA / 255)), Line.c_str());
			}
			ImGui::GetBackgroundDrawList()->AddText(ImGui::GetFont(), size, ImVec2(pos.x, pos.y + TextSize.y * I), color, Line.c_str());
		}

		Y = pos.y + TextSize.y * (I + 1);
		I++;
	}
	ImGui::PopFont();
}
