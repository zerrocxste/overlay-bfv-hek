class c_notification_panel
{
private:
	int m_position;
	int m_max_time;
	struct notification_s
	{
		notification_s(int type, std::string message, std::string title) :
			m_type(type), m_message(message), m_title(title), m_is_init_notification(false), m_timestamp(0), m_alpha(1.f), m_offset_pos_x(0) { };
		int m_type;
		std::string m_message, m_title;
		bool m_is_init_notification;
		DWORD m_timestamp;
		float m_alpha;
		int m_offset_pos_x;
	};
	std::deque<notification_s>m_notification_data;
public:
	enum POS {
		BOTTOM_RIGHT,
		TOP_RIGHT,
		BOTTOM_LEFT,
		TOP_LEFT
	};

	c_notification_panel()
	{
		this->m_position = BOTTOM_RIGHT;
		this->m_max_time = 5000;
	}

	c_notification_panel(POS pos)
	{
		this->m_position = pos;
	}

	void set_position(POS pos)
	{
		this->m_position = pos;
	}

	void set_timer(int time_ms)
	{
		this->m_max_time = time_ms;
	}

	void push_notification(int type, const std::string message, const std::string title)
	{
		this->m_notification_data.emplace_back(type, message, title);
	}

	void render()
	{
		if (this->m_notification_data.empty())
			return;

		auto* _front = &this->m_notification_data.front();

		auto size = ImGui::CalcTextSize(_front->m_message.c_str());
		size.x = ImClamp(size.x, 100.f, 500.f);

		ImGui::SetNextWindowSize(ImVec2(size.x + 40.f, size.y + 40.f));

		if (!_front->m_is_init_notification)
		{
			_front->m_is_init_notification = true;
			_front->m_offset_pos_x = size.x + 70;
			_front->m_timestamp = GetTickCount();
		}

		if (_front->m_offset_pos_x > 0)
			_front->m_offset_pos_x -= 5;

		switch (m_position)
		{
		case POS::BOTTOM_RIGHT:
			ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - size.x - 70.f + _front->m_offset_pos_x, ImGui::GetIO().DisplaySize.y - size.y - 70.f));
			break;
		case POS::TOP_RIGHT:
			ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - size.x - 70.f + _front->m_offset_pos_x, 30.f));
			break;
		case POS::BOTTOM_LEFT:
			ImGui::SetNextWindowPos(ImVec2(30.f - _front->m_offset_pos_x, ImGui::GetIO().DisplaySize.y - size.y - 70.f));
			break;
		case POS::TOP_LEFT:
			ImGui::SetNextWindowPos(ImVec2(30.f - _front->m_offset_pos_x, 30.f));
			break;
		default:
			return;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);

		if (GetTickCount() - _front->m_timestamp >= (this->m_max_time - 300))
			_front->m_alpha -= 0.065f;

		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, _front->m_alpha);

		ImGui::Begin(_front->m_title.c_str(), nullptr,
			ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
		ImGui::Text(_front->m_message.c_str());
		ImGui::End();

		ImGui::PopStyleVar(2);

		if (GetTickCount() - _front->m_timestamp >= this->m_max_time)
			this->m_notification_data.pop_front();
	}
};
extern std::unique_ptr<c_notification_panel> g_up_notification_panel;