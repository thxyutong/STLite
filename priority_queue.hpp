#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {

template<typename T, class Compare = std::less<T> >
class priority_queue {
	struct node {
		node *lson, *rson;
		T data;
		int npl;
		node(const T &_data, int _npl = 0, node *_lson = NULL, node *_rson = NULL) :
			data(_data), npl(_npl), lson(_lson), rson(_rson) {
			getNpl();	
		}
		void getNpl() {
			if(rson == NULL) npl = 0;
			else npl = rson->npl + 1;
		}
		static node * copy(const node *other) {
			if(!other) return NULL;
			else return new node(other->data, other->npl, copy(other->lson), copy(other->rson));
		}
		static void destroy(node *now) {
			if(!now) return;
			if(now->lson != NULL) destroy(now->lson);
			if(now->rson != NULL) destroy(now->rson);
			delete now;
		}
		static void swap(node *&x, node *&y) {
			node *z = x;
			x = y, y = z;
		}
		static node * merge(node *x, node *y) {
			if(!x) return y;
			else if(!y) return x;
			Compare cmp;
			if(cmp(x->data, y->data)) swap(x, y);
			x->rson = merge(x->rson, y);
			if(!x->lson || x->lson->npl < x->rson->npl) swap(x->lson, x->rson);
			x->getNpl();
			return x;
		}
	};
	node *root;
	int heapSize;
public:
	priority_queue() {
		root = NULL;
		heapSize = 0;
	}
	priority_queue(const priority_queue &other) {
		root = node::copy(other.root);
		heapSize = other.heapSize;
	}
	~priority_queue() {
		node::destroy(root);
	}
	priority_queue &operator=(const priority_queue &other) {
		if(this == &other) return *this;
		node::destroy(root);
		root = node::copy(other.root);
		heapSize = other.heapSize;
		return *this;
	}
	const T & top() const {
		if(empty()) throw container_is_empty();
		else return root->data;
	}
	void push(const T &e) {
		node *now = new node(e);
		root = node::merge(root, now);
		++heapSize;
	}
	void pop() {
		if(empty()) throw container_is_empty();
		node *tmp = node::merge(root->lson, root->rson);
		delete root;
		root = tmp;
		--heapSize;
	}
	size_t size() const {
		return heapSize;
	}
	bool empty() const {
		return root == NULL;
	}
	void merge(priority_queue &other) {
		root = node::merge(root, other.root);
		heapSize = heapSize + other.heapSize;
		other.root = NULL;
		other.heapSize = 0;
	}
};

}
#endif
