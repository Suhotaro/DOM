// DOM.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <memory>
#include <fstream>
#include <sstream>

using namespace std;

class DOM
{
private:
	string fileName;
	ifstream in;

	struct data {
		int from;
		int to;

		data(int _from = 0, int _to = 0)
			: from(_from), to(_to)
		{}

		void set_from(int _from) { from = _from; }
		void set_to(int _to) { to = _to; }
	};

	struct node {
		string nodeName;
		shared_ptr<struct data> data;

		shared_ptr<struct node> parrent;
		shared_ptr<struct node> next;
		shared_ptr<struct node> child;

		node(string _name,
			shared_ptr<struct node> _parrent,
			shared_ptr<struct node> _next,
			shared_ptr<struct node> _child)
			: nodeName(_name), parrent(_parrent), next(_next), child(_child)
		{}

		node(string _name,
			shared_ptr<struct node> _parrent)
			: nodeName(_name), parrent(_parrent)
		{}

		node(string _name)
			: nodeName(_name)
		{}

		void setData(shared_ptr<struct data> _data) { data = _data; }
		shared_ptr<struct data> getData() { return data; }
	};

	shared_ptr<struct node> head;

public:
	explicit DOM(std::string fileName_) : fileName(fileName_) {
		in.open(fileName, ifstream::in);
		if (in.fail()) {
			printf("ERROR: open file\n");
			exit(0);
		}
	}

	void buildDOMStructure() {
		readNode("init", NULL);
	}

	void showDOMStructure() {		
		showRootTree(head, "");		
	}

	void startTreep() {
		mainProccessing(head);
	}

private:
	void readNode(string name, shared_ptr<struct node> parrentNode) {
		string line;
		string prev;
		string current = name;

		std::istringstream iss;
		prev = current;
		shared_ptr<struct node> newNode;

		if (string("init").length() == name.length() && 0 == name.compare("init")) {
			current = readNextWord(iss, line);
			name = current;
			prev = current;

			newNode = make_shared<node>(name, parrentNode, nullptr, nullptr);
			head = newNode;
		} else {
			newNode = createNode(current, parrentNode);
		}

		while (!in.eof())
		{
			current = readNextWord(iss, line);

			if (isData(current)) {
				processData(iss, line, newNode);
				current = readNextWord(iss, line);
			}

			if (isEndingWord(name, current))
				return;

			readNode(current, newNode);

			prev = current;
		}
	}

	string readNextWord(std::istringstream &iss, string &line) {
		iss.clear();
		getline(in, line);
		iss.str(line.c_str());

		string word;
		iss >> word;

		return word;
	}

	shared_ptr<struct node> createNode(string name, shared_ptr<struct node> parrent) {
		shared_ptr<struct node> newNode = make_shared<node>(name, parrent);
		insertNodeToRootTree(newNode, parrent);
		return newNode;
	}

	void insertNodeToRootTree(shared_ptr<struct node> newNode, shared_ptr<struct node> parrent) {
		newNode->parrent = parrent;

		if (nullptr == parrent->child)
		{
			parrent->child = newNode;
			return;
		}

		shared_ptr<struct node> iter = parrent->child;
		while (iter->next)
			iter = iter->next;
		iter->next = newNode;
	}

	bool isData(string word) {
		if (word[0] != '<')
			return true;
		return false;
	}

	void processData(std::istringstream & iss, string &line, shared_ptr<struct node> node) {
		shared_ptr<struct data> data(new data);

		iss.clear();
		iss.str(line.c_str());

		string value;
		iss >> value;
		data->set_from(atoi(value.c_str()));

		iss >> value;
		data->set_to(atoi(value.c_str()));

		node->data = data;
	}

	bool isEndingWord(string beginingTag, string endingTag) {
		if (beginingTag.size() == (endingTag.size() - 1) &&
			0 == beginingTag.compare(1, beginingTag.size() - 2, endingTag, 2, endingTag.size() - 3))
			return true;
		return false;
	}

	void showRootTree(shared_ptr<struct node> node, string indent) {
		if (nullptr == node)
			return;

		showNode(node);

		showRootTree(node->child, indent + "  ");
		showRootTree(node->next, indent);
	}

	void showNode(shared_ptr<struct node> node) {
		showNodeName(node->nodeName);
		if (node->data)
			showNodeData(node->data->from, node->data->to);
	}

	virtual void showNodeName(string name) {
		printf("Node\n  name:%s\n", name.c_str());
	}

	virtual void showNodeData(int from, int to) {
		printf("  data:%d %d\n", from, to);
	}

	char mainProccessing(shared_ptr<struct node> node) {
		if (node == nullptr) {
			cout << "The node is NULL, return\n";
			return 'x';
		}

		nodeInfo(node);
		char cmd = readNextCommand();
		while (1)
		{
			switch (cmd)
			{
				case 'n': cmd = mainProccessing(node->next); break;
				case 'p': cmd = mainProccessing(node->parrent); break;
				case 'c': cmd = mainProccessing(node->child); break;
			}

			if (cmd == 'e') {
				cout << "Exit\n";
				return 'e';
			}				

			nodeInfo(node);
			cmd = readNextCommand();
		}				
	}

	char readNextCommand() {
		printf("Info:\n  n - next\n  p - parrent\n  c - child\n  e - exit\n");
		printf("enter next way:");

		string str;
		getline(cin, str);

		return str.at(0);
	}
		
	void nodeInfo(shared_ptr<struct node> node) {
		printf("Node:\n  name:%s\n", node->nodeName.c_str());
		if (node->data)
			printf("  data:%d %d\n", node->data->to, node->data->from);
	}
};

int main()
{
	DOM dom("input.txt");
	dom.buildDOMStructure();
	dom.showDOMStructure();

	cout << "Root tree has been build\n" << endl;

	dom.startTreep();

	return 0;
}

