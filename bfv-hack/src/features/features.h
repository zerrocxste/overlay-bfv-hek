namespace features
{
	void initialize_features();

	void notification();

	void draw();
	void cse_list_grabber(DWORD_PTR cse);
	void local_cse_grabber(DWORD_PTR p_cse);
	void draw_cringeshot_counter();

	void norecoil(bool is_enable);
	void nospread(bool is_enable);
	void increase_fire_rate(bool is_enable, float rate = 0.02f);
}