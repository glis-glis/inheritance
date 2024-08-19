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

struct TPerLine {
	virtual std::string info() const = 0;
	virtual void apply(std::string& String) = 0;
	virtual std::string summary() const = 0;

	virtual ~TPerLine() = default;
};

class TStringTraverser {
private:
	std::string _fileName;
	std::vector<std::string> _strings;
	std::unique_ptr<TPerLine> _perLine;

public:
	TStringTraverser(std::string_view Filename, TPerLine* PerLine): _fileName(Filename), _perLine(PerLine) {
		std::ifstream iFile((std::string(Filename)));
		std::string line;
		while(std::getline(iFile, line)) {
			_strings.push_back(line);
		}
	}

	void run() {
		writeln(_perLine->info(), " of File '", _fileName, "'.");
		for (auto& s: _strings) {
			_perLine->apply(s);
		}
		writeln(_perLine->summary(), " in File '", _fileName, "'.");
	}
};

class TLineCounter : public TPerLine {
private:
	size_t _nLines = 0;

public:
	void apply(std::string& String) override {
		++_nLines;
	}

	std::string info() const override {
		return "Counting number of lines";
	}

	std::string summary() const override {
		return "Number of lines = " + std::to_string(_nLines);
	}
};

class TSpecificCounter : public TPerLine {
private:
	size_t _nLines = 0;
	size_t _size   = 0;


public:
	TSpecificCounter(size_t Size) : _size(Size) {}

	void apply(std::string& String) override {
		if (String.size() == _size) ++_nLines;
	}

	std::string info() const override {
		return "Counting number of lines with length = "  + std::to_string(_size);
	}

	std::string summary() const override {
		return "Number of lines with length = " + std::to_string(_size) + " is " + std::to_string(_nLines);
	}
};

int main(int argc, char *argv[]) {
	if (argc != 3) {
		writeln("Usage: ", argv[0], " <type> <file>");
		return 1;
	}

	const std::string_view task = argv[1];
	const std::string_view file = argv[2];

	std::unique_ptr<TPerLine> perLine;

	if (task == "count") {
		perLine = std::make_unique<TLineCounter>();
	} else if (std::isdigit(task.front())) {
		perLine = std::make_unique<TSpecificCounter>(task.front() - '0');
	} else {
		writeln("Task '", task, "' is unknown!");
		return 1;
	}

	TStringTraverser traverser(file, perLine.release());
	traverser.run();
	return 0;
}
