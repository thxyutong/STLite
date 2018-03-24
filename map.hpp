#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"
namespace sjtu {
template<class Key, class T, class Compare = std::less<Key> > class map {
public:
	typedef pair<const Key, T> value_type;
private:
	friend class iterator;
	friend class const_iterator;
	Compare comparator;
	struct node {
		value_type *value;
		node *father, *child[2]; // 0-lson, 1-rson
		node *next[2];
		size_t size;
		node(value_type *_value) : value(_value), father(NULL), size(1) {
			next[0] = next[1] = NULL;
			child[0] = child[1] = NULL;
		}
		~node() {if(value != NULL) delete value;}
		void delink() {
			if(next[1]) next[1]->next[0] = next[0];
			if(next[0]) next[0]->next[1] = next[1];
		}
		void enlink(node *_next, node *_prev) {
			next[1] = _next;
			next[0] = _prev;
			if(next[1]) next[1]->next[0] = this;
			if(next[0]) next[0]->next[1] = this;
		}
	};
	void destroy(node *now) {
		if(now == NULL) return;
		destroy(now->child[0]);
		destroy(now->child[1]);
		delete now;
	}
	size_t get_size(node *ptr) {return ptr ? ptr->size : 0;}
	void rotate(node *&now, bool type) { // 0-left, 1-right
		node *tmp = now->child[!type];
		now->child[!type] = tmp->child[type];
		if(now->child[!type]) now->child[!type]->father = now;
		tmp->child[type] = now;
		tmp->size = now->size;
		now->size = get_size(now->child[0]) + get_size(now->child[1]) + 1;
		tmp->father = now->father;
		now->father = tmp;
		now = tmp;
	}
	void balance(node *&now, bool type) { // 1-rson_deeper
		if(now->child[type] == NULL) return;
		if(get_size(now->child[type]->child[type]) > get_size(now->child[!type])) rotate(now, !type);
		else if(get_size(now->child[type]->child[!type]) > get_size(now->child[!type])) rotate(now->child[type], type), rotate(now, !type);
			 else return;
		balance(now->child[0], 0);
		balance(now->child[1], 1);
		balance(now, 0);
		balance(now, 1);
	}
	bool cmp(node *x, const Key &y) const {
		if(x->value == NULL) return false;
		return comparator(x->value->first, y);
	}
	bool cmp(const Key &x, node *y) const {
		if(y->value == NULL) return true;
		return comparator(x, y->value->first);
	}
	node *next(node *now, bool type, const Key &key) const {
		if(now == NULL) return NULL;
		if(type ? cmp(now, key) : cmp(key, now)) return next(now->child[type], type, key);
		else {
			node *tmp = next(now->child[!type], type, key);
			return tmp == NULL ? now : tmp;
		}
	}
	node *insert(node *&now, const value_type &value, node *father) {
		if(now == NULL) {
			node *tmp = new node(new value_type(value));
			tmp->enlink(next(root, true, value.first), next(root, false, value.first));
			tmp->father = father;
			return now = tmp;
		}
		++now->size;
		node *rtn = insert(now->child[cmp(now, value.first)], value, now);
		balance(now, cmp(now, value.first));
		return rtn;
	}
	void swap(node *&x, node *&y) {
		node tx = *x, ty = *y;
		tx.value = ty.value = NULL;
		x->size = ty.size;
		y->size = tx.size;
		if(x == y->father) {
			x->father = y;
			if(y == tx.child[0]) y->child[0] = x, y->child[1] = tx.child[1];
			else y->child[1] = x, y->child[0] = tx.child[0];
		} else {
			x->father = ty.father;
			if(y == y->father->child[0]) y->father->child[0] = x;
			else y->father->child[1] = x;
			y->child[0] = tx.child[0];
			y->child[1] = tx.child[1];
		}
		x->child[0] = ty.child[0];
		x->child[1] = ty.child[1];
		y->father = tx.father;
		if(x->child[0]) x->child[0]->father = x;
		if(x->child[1]) x->child[1]->father = x;
		if(y->child[0]) y->child[0]->father = y;
		if(y->child[1]) y->child[1]->father = y;
		node *t = y; y = x, x = t;
	}
	void erase(node *&now, const Key &key) {
		--now->size;
		if(cmp(now, key)) {erase(now->child[1], key); return;}
		if(cmp(key, now)) {erase(now->child[0], key); return;}
		if(now->child[0] && now->child[1]) {
			node *tmp = next(now->child[0], false, key);
			swap(now, tmp);
			erase(now->child[0], key);
			return;
		}
		node *tmp = now->child[0] ? now->child[0] : now->child[1];
		if(tmp) tmp->father = now->father;
		now->delink();
		delete now;
		now = tmp;
	}
	node *find(node *now, const Key &key) const {
		if(now == NULL || !(cmp(now, key) || cmp(key, now))) return now;
		return find(now->child[cmp(now, key)], key);
	}
	node *root, *finish;
	size_t current_size;
public:
	class const_iterator;
	class iterator {
	private:
		friend class map;
		node *node_ptr;
		map *belong;
	public:
		iterator(node *_node_ptr = NULL, const map *_belong = NULL) : 
			node_ptr(_node_ptr), belong((map*)_belong) {}
		iterator operator++(int) {
			iterator rtn = *this;
			operator++();
			return rtn;
		}
		iterator & operator++() {
			if(node_ptr->next[1] == NULL) throw index_out_of_bound();
			node_ptr = node_ptr->next[1];
			return *this;
		}
		iterator operator--(int) {
			iterator rtn = *this;
			operator--();
			return rtn;
		}
		iterator & operator--() {
			if(node_ptr->next[0] == NULL) throw index_out_of_bound();
			node_ptr = node_ptr->next[0];
			return *this;
		}
		value_type & operator*() const {
			if(node_ptr == NULL || node_ptr->value == NULL) throw invalid_iterator();
			return *node_ptr->value;
		}
		bool operator==(const iterator &rhs) const {
			if(node_ptr != rhs.node_ptr) return false;
			if(belong != rhs.belong) return false;
			return true;
		}
		bool operator==(const const_iterator &rhs) const {
			if(node_ptr != rhs.node_ptr) return false;
			if(belong != rhs.belong) return false;
			return true;
		}
		bool operator!=(const iterator &rhs) const {return !operator==(rhs);}
		bool operator!=(const const_iterator &rhs) const {return !operator==(rhs);}
		value_type* operator->() const noexcept {return node_ptr->value;}
	};
	class const_iterator {
	private:
		friend class map;
		const node *node_ptr;
		const map *belong;
	public:
		const_iterator(node *_node_ptr = NULL, const map *_belong = NULL) : 
			node_ptr(_node_ptr), belong(_belong) {}
		const_iterator(const iterator &other) : node_ptr(other.node_ptr), belong(other.belong) {}
		const_iterator &operator=(const iterator &other) {
			node_ptr = other.node_ptr;
			belong = other.belong;
			return *this;
		}
		const_iterator operator++(int) {
			const_iterator rtn = *this;
			operator++();
			return rtn;
		}
		const_iterator & operator++() {
			if(node_ptr->next[1] == NULL) throw index_out_of_bound();
			node_ptr = node_ptr->next[1];
			return *this;
		}
		const_iterator operator--(int) {
			const_iterator rtn = *this;
			operator--();
			return rtn;
		}
		const_iterator & operator--() {
			if(node_ptr->next[0] == NULL) throw index_out_of_bound();
			node_ptr = node_ptr->next[0];
			return *this;
		}
		const value_type & operator*() const {
			if(node_ptr == NULL || node_ptr->value == NULL) throw invalid_iterator();
			return *node_ptr->value;
		}
		bool operator==(const iterator &rhs) const {
			if(node_ptr != rhs.node_ptr) return false;
			if(belong != rhs.belong) return false;
			return true;
		}
		bool operator==(const const_iterator &rhs) const {
			if(node_ptr != rhs.node_ptr) return false;
			if(belong != rhs.belong) return false;
			return true;
		}
		bool operator!=(const iterator &rhs) const {return !operator==(rhs);}
		bool operator!=(const const_iterator &rhs) const {return !operator==(rhs);}
		const value_type* operator->() const noexcept {return node_ptr->value;}
	};
	void copy(const map &other) {
		if(this == &other) return;
		clear();
		for(const_iterator it = other.cbegin(); it != other.cend(); ++it) insert(*it);
	}
	map() {root = NULL; clear();}
	map(const map &other) : map() {copy(other);}
	map & operator=(const map &other) {copy(other); return *this;}
	~map() {clear(); delete root;}
	T & at(const Key &key) {
		node *node_ptr = find(root, key);
		if(node_ptr == NULL) throw index_out_of_bound();
		return (*node_ptr->value).second;
	}
	const T & at(const Key &key) const {
		node *node_ptr = find(root, key);
		if(node_ptr == NULL) throw index_out_of_bound();
		return (*node_ptr->value).second;
	}
	T & operator[](const Key &key) {
		node *node_ptr = find(root, key);
		if(node_ptr == NULL) {
			++current_size;
			node_ptr = insert(root, value_type(key, T()), NULL);
		}
		return (*node_ptr->value).second;
	}
	const T & operator[](const Key &key) const {return at(key);}
	iterator begin() {
		node *node_ptr = root;
		while(node_ptr->child[0]) node_ptr = node_ptr->child[0];
		return iterator(node_ptr, this);
	}
	const_iterator cbegin() const {
		node *node_ptr = root;
		while(node_ptr->child[0]) node_ptr = node_ptr->child[0];
		return const_iterator(node_ptr, this);
	}
	iterator end() {return iterator(finish, this);}
	const_iterator cend() const {return const_iterator(finish, this);}
	bool empty() const {return current_size == 0;}
	size_t size() const {return current_size;}
	void clear() {
		current_size = 0;
		destroy(root);
		finish = root = new node(NULL);
	}
	pair<iterator, bool> insert(const value_type &value) {
		iterator it = find(value.first);
		if(it == end()) {
			++current_size;
			return pair<iterator, bool>(iterator(insert(root, value, NULL), this), true);
		} else return pair<iterator, bool>(it, false);
	}
	void erase(iterator pos) {
		if(pos.belong != this) throw invalid_iterator();
		Key key = (*pos).first;
		if(find(root, key) == NULL) throw invalid_iterator();
		--current_size;
		erase(root, key);
	}
	size_t count(const Key &key) const {
		if(find(root, key) == NULL) return 0;
		else return 1;
	}
	iterator find(const Key &key) {
		node *node_ptr = find(root, key);
		if(node_ptr == NULL) return end();
		else return iterator(node_ptr, this);
	}
	const_iterator find(const Key &key) const {
		node *node_ptr = find(root, key);
		if(node_ptr == NULL) return cend();
		else return const_iterator(node_ptr, this);
	}
};
}
#endif