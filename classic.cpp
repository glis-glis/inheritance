#include <fstream>
#include <iostream>
#include <memory>
#include <vector>
#include <string>

void writeln() noexcept {
	std::cout << '\n';
}

template<typename T, typename... Ts>
void writeln(const T& t, const Ts&... ts) noexcept {
	std::cout << t;
	writeln(ts...);
}

class TStringTraverser {
private:
	std::string _fileName;
	std::vector<std::string> _strings;
	virtual void _doSomething(std::string& String) = 0; // ! String may be mutated !

public:
	TStringTraverser(std::string_view Filename): _fileName(Filename) {
		std::ifstream iFile((std::string(Filename)));
		std::string line;
		while(std::getline(iFile, line)) {
			_strings.push_back(line);
		}
	}
	virtual ~TStringTraverser() = default;

	void traverse() {
		for (auto& s: _strings) {
			_doSomething(s);
		}
	}

	virtual void run() = 0;

	const std::string& fileName() const noexcept {
		return _fileName;
	}
};

class TLineCounter : public TStringTraverser {
private:
	size_t _nLines = 0;
	void _doSomething(std::string& String) override {
		++_nLines;
	}
public:
	TLineCounter(std::string_view Filename) : TStringTraverser(Filename) {}
	void run() override {
		writeln("Counting number of lines of File '", fileName(), "'.");
		traverse();
		writeln("Number of lines in File '", fileName(), "':", _nLines, ".");
	}
};

class TSpecificCounter : public TStringTraverser {
private:
	size_t _nLines = 0;
	size_t _size   = 0;

	void _doSomething(std::string& String) override {
		if (String.size() == _size) ++_nLines;
	}

public:
	TSpecificCounter(std::string_view Filename, size_t Size) : TStringTraverser(Filename), _size(Size) {}
	void run() override {
		writeln("Counting number of lines with length = ", _size, " of File '", fileName(), "'.");
		traverse();
		writeln("Number of lines with length = ", _size, " in File '", fileName(), "':", _nLines, ".");
	}
};

int main(int argc, char *argv[]) {
	if (argc != 3) {
		writeln("Usage: ", argv[0], " <type> <file>");
		return 1;
	}

	const std::string_view task = argv[1];
	const std::string_view file = argv[2];

	std::unique_ptr<TStringTraverser> traverser;

	if (task == "count") {
		traverser = std::make_unique<TLineCounter>(file);
	} else if (std::isdigit(task.front())) {
		traverser = std::make_unique<TSpecificCounter>(file, task.front() - '0');
	} else {
		writeln("Task '", task, "' is unknown!");
		return 1;
	}

	traverser->run();
	return 0;
}
