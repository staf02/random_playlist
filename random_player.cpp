#include <iostream>
#include "dirent.h"
#include <random>
#include <string>
#include <algorithm>
#include <set>
#include <fstream>
#include <vector>
#include <windows.h>

#pragma warning (disable : 4996)

const std::string codecs[] = {
	".flac",
	".mp3"
};

std::string cp1251_to_utf8(const char* str) {
	std::string res;
	int result_u, result_c;
	result_u = MultiByteToWideChar(1251, 0, str, -1, 0, 0);
	if (!result_u) { 
		return 0;
	}
	wchar_t* ures = new wchar_t[result_u];
	if (!MultiByteToWideChar(1251, 0, str, -1, ures, result_u)) {
		delete[] ures;
		return 0;
	}
	result_c = WideCharToMultiByte(65001, 0, ures, -1, 0, 0, 0, 0);
	if (!result_c) {
		delete[] ures;
		return 0;
	}
	char* cres = new char[result_c];
	if (!WideCharToMultiByte(65001, 0, ures, -1, cres, result_c, 0, 0)) {
		delete[] cres;
		return 0;
	}
	delete[] ures;
	res.append(cres);
	delete[] cres;
	return res;
}

bool is_pref(std::string& const source, std::string& const pattern) {
	return source.find(pattern) == 0;
}

void run(std::string& const path_to_player) {
    std::string path = "play.m3u";
	std::string cmd = "";
	cmd += path_to_player;
    cmd += " ";
    cmd += '"';
    cmd += path;
    cmd += '"';
    system(cmd.c_str());
}

bool is_song(std::string& const file_name) {
	for (std::string codec_name : codecs) {
		if (file_name.length() >= codec_name.length() && file_name.find(codec_name) == file_name.length() - codec_name.length()) {
			return true;
		}
	}
	return false;
}

std::set<std::string> songs;

void go(std::string path, std::string pref) {
	if (!is_pref(path, pref))
		return;
	if (is_song(path)) {
		songs.insert(path);
		return;
	}
	struct dirent** namelist;
	int n;
	const char* c = path.c_str();
	n = scandir(c, &namelist, 0, alphasort);
	int i = 2;
	while (i < n) {
		std::string s = namelist[i]->d_name;
		std::string p1 = path + "/" + s;
		go(p1, pref);
		i++;
	}
}

bool random = false;

void generate_play() {
	std::vector<std::string> s;
	for (std::string t : songs) {
		s.push_back(t);
	}
	if (random) {
		random_shuffle(s.begin(), s.end());
	}
	std::string res = "";
	for (std::string t : s) {
		res += t;
		res += "\n";
	}
	res = cp1251_to_utf8(res.c_str());
	std::ofstream cout("play.m3u");
	cout << res;
}

void input(std::string& const fname, std::string& const path_to_music) {
	std::ifstream cin;
	cin.open(fname.c_str());
	char c = 'a';
	while (cin.get(c)) {
		if (c == '+') {
			cin.get();
			std::string s;
			getline(cin, s);
			s = path_to_music + s;
			go(path_to_music, s);
		}
		else if (c == '-') {
			cin.get();
			std::string s;
			getline(cin, s);
			s = path_to_music + s;
			std::set<std::string> new_songs;
			for (std::string t : songs) {
				if (!is_pref(t, s))
					new_songs.insert(t);
			}
			songs = new_songs;
		}
	}
	cin.close();
	return;
}

int main() {
	SetConsoleCP(1251);
	srand(time(0));
	setlocale(0, "rus");
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(0);
	std::cout.tie(0);
	std::string path_to_music, path_to_player;
	std::cout << "Enter your music path\n";
	getline(std::cin, path_to_music);
	std::cout << "Enter your music player path\n";
	getline(std::cin, path_to_player);
	char cmd_input;
	while (true) {
		std::cin >> cmd_input;
		if (cmd_input == '!') {
			generate_play();
			run(path_to_player);
		}
		else if (cmd_input == '+') {
			std::cin.get();
			std::string input_folder;
			getline(std::cin, input_folder);
			input_folder = cp1251_to_utf8(input_folder.c_str());
			input_folder = path_to_music + "/" + input_folder;
			go(input_folder, input_folder);
		}
		else if (cmd_input == '-') {
			std::cin.get();
			std::string input_folder;
			getline(std::cin, input_folder);
			input_folder = path_to_music + input_folder;
			std::set<std::string> new_songs;
			for (std::string t : songs) {
				if (!is_pref(t, input_folder))
					new_songs.insert(t);
			}
			songs = new_songs;
		}
		else if (cmd_input == '#') {
			std::cin.get();
			std::string input_folder;
			getline(std::cin, input_folder);
			input_folder = "pl/" + input_folder + ".txt";
			input(input_folder, path_to_music);
		}
		else if (cmd_input == 'R') {
			random = true;
		}
	}
}