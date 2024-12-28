////////////////////////////////////////////////////////////////////////////////
//
// Implementation of Knuth's Dancing Links algorithm, Matthew Arcus, 2024
// MIT license
//
// Input is read from stdin and is the form:
// 10010010101
// 00100010001
// ...
// 
// each line being a set of options for the exact cover problem, so should all
// have the same length. Blank lines and lines beginning with # are ignored.
//
// There are no command line parameters (yet).
//
// When a solution is found involving a sequence of option o1,o2,o3, a string
// is written (to stdout) containing '1' for the bits set in o1,'2' for bits
// set in o2 etc.
//
// Some diagostic information is written to stderr.
//
// See Makefile for some sample inputs
//
////////////////////////////////////////////////////////////////////////////////


#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

// Suggested data structure is a tabular structure of nodes,
// we will encapsulate this in instances of class DLink.
template <typename T>
class DLink {
public:
  // Need room for a certain number of items, so specify that
  DLink(int nitems) : ncount(0) {
    init(nitems);
  }
  // And we need a node class, we have data and the two links
  // for the linked list structure. We will call them prev and
  // next and define functions to access as rlink, ulink etc.
  struct Node {
    Node(T data_, T prev_, T next_)
      : data(data_), prev(prev_), next(next_) {}
    Node() {}
    T data; T prev; T next;
  };
  // And we need somewhere to put our nodes. We will have a
  // a header array and a node array, both represented as
  // std::vectors
protected:
  std::vector<Node> headers;
  std::vector<Node> nodes;
  int nrows;
  long ncount;
  
  // Various accessors with names to correspond to Knuth's description
  // Fields that shouldn't be modified after creation don't return reference.
  T name(int i) const { return headers[i].data; }
  T &llink(int i) { return headers[i].prev; }
  T &rlink(int i) { return headers[i].next; }
  T &len(int i) { return nodes[i].data; }
  T top(int i) { return nodes[i].data; }
  T &ulink(int i) { return nodes[i].prev; }
  T &dlink(int i) { return nodes[i].next; }

  int nnodes() { return nodes.size(); }
  int lastindex() { return nodes.size()-1; }
  bool isspacer(int i) { return top(i) <= 0; }

  // Initialize the structure correctly
  void init(int nitems) {
    nrows = 0;
    headers.resize(nitems+1);
    nodes.resize(nitems+1);
    for (int i = 0; i <= nitems; i++) {
      int previous = (i+nitems)%(nitems+1);
      int next = (i+1)%(nitems+1);
      headers[i] = Node(i,previous,next);
      nodes[i] = Node(0,i,i); // Empty list, zero length
    }
    nodes.push_back(Node(0,0,0)); // First spacer
  }

  void hide(int p) {
    int q = p+1;
    while (q != p) {
      int x = top(q), u = ulink(q), d = dlink(q);
      if (x <= 0) {
        q = u;
      } else {
        dlink(u) = d; ulink(d) = u;
        len(x)--; q++;
      }
    }
  }
  void unhide(int p) {
    int q = p-1;
    while (q != p) {
      int x = top(q), u = ulink(q), d = dlink(q);
      if (x <= 0) {
        q = d;
      } else {
        dlink(u) = q; ulink(d) = q;
        len(x)++; q--;
      }
    }
  }
  void cover(int i) {
    int p = dlink(i);
    while (p != i) {
      hide(p);
      p = dlink(p);
    }
    int l = llink(i), r = rlink(i);
    rlink(l) = r; llink(r) = l;
  }
  void uncover(int i) {
    int l = llink(i), r = rlink(i);
    rlink(l) = i; llink(r) = i;
    int p = ulink(i);
    while (p != i) {
      unhide(p);
      p = ulink(p);
    }
  }
public:
  int nitems() { return headers.size()-1; }
  void dprint() {
    for (int i = 0; i < 1+nitems(); i++) {
      printf("(%2d %2d %2d)",name(i),llink(i),rlink(i));
    }
    printf("\n");
    for (int i = 0; i < nnodes(); i++) {
      printf("(%2d %2d %2d)",len(i),ulink(i),dlink(i));
      if ((i+1)%8 == 0 || i+1 == nnodes()) printf("\n");
    }
    printf("\n");
  }
  void stats() {
    fprintf(stderr,"%ld items, %ld rows, %ld nodes\n",
            (long)nitems(), (long)nrows, (long)nodes.size());
  }
  void print() {
    int start = 0;
    for (int i = rlink(start); i != start; i = rlink(i)) {
      assert(llink(rlink(i)) == i);
      assert(rlink(llink(i)) == i);
      printf("%d", i);
    }
    printf("\n");
    for (int i = rlink(start); i != start; i = rlink(i)) {
      printf("%d", len(i));
    }
    printf("\n");
    int spacer = nitems()+1;
    assert(top(spacer) == 0); // Check first spacer
    for ( ; dlink(spacer) != 0; spacer = dlink(spacer)+1) {
      int node = spacer+1;
      for (int i = rlink(start); i != start; i = rlink(i)) {
        while(top(node) > 0 && top(node) < i) node++;
        if (top(node) == i) printf("1");
        else printf("0");
      }
      printf("\n");
    }
    printf("\n");
  }
  // Now add a row. We will supply a string of 0s and 1s that
  // indicate the row members (since this will be our input
  // format).
  void addrow(const std::string &s) {
    assert((int)s.size() <= nitems());
    // The last node in the nodes array is a spacer node,
    // whose dlink must be set to the last node in the
    // row being inserted, and the nodes items will have a
    // new spacer node inserted afterwards, whose ulink
    // is start of the row.
    // Assume the row is non-empty (in fact, assert this).
    int spacer = lastindex();
    assert(isspacer(spacer));
    int rowstart = spacer+1;
    for (int i = 0; i < (int)s.size(); i++) {
      if (s[i] == '1') {
        int item = i+1;
        nodes.push_back(Node(item,ulink(item),item));
        int index = lastindex();
        dlink(ulink(index)) = index;
        ulink(dlink(index)) = index;
        len(item)++;
      }
    }
    int rowend = lastindex();
    assert(rowend >= rowstart);
    nodes.push_back(Node(len(spacer)-1,rowstart,0)); // New spacer
    dlink(spacer) = rowend;
    nrows++;
  }

  int chooseitem() {
    int i = rlink(0);
    int best = i;
    int bestlen = len(i);
    while(i != 0) {
      if (len(i) < bestlen) {
        bestlen = len(i); best = i;
      }
      i = rlink(i);
    }
    return best;
  }

  // Knuth, of course, gives the algorithm in a non-recursive form
  // but it's maybe clearer to give a recursive variant (and
  // make little difference to efficiency - most of the time is
  // spend covering and uncovering nodes).
  template <typename Visitor>
  void rdance(Visitor &visitor, std::vector<int> &stack) {
    ncount++;
    if (rlink(0) == 0) {
      visitor.visit(*this); // All items accounted for
    } else {
      int i = chooseitem(); // Select an uncovered item
      cover(i);             // Cover it
      for (int k = dlink(i); k != i; k = dlink(k)) {
        // Now go through all options containing item i
        assert(top(k) == i);
        for (int p = k+1; p != k; ) {
          int j = top(p);
          if (j <= 0) {
            p = ulink(p); // Spacer node, so back to beginning of row.
          } else {
            cover(j); // Item j now covered, so remove it
            p = p+1;  // Next item
          }
        }
        stack.push_back(k);
        rdance<Visitor>(visitor,stack);
        stack.pop_back();
        // Uncover covered items (in reverse order)
        for (int p = k-1; p != k; ) {
          int j = top(p);
          if (j <= 0) {
            p = dlink(p);
          } else {
            uncover(j);
            p = p-1;
          }
        }
      }
      uncover(i);
    }
  }

  int getoption(int p) {
    while (top(p) >= 0) p++; // Find spacer node
    return -top(p)-1; // Return zero-based option index
  }
  long int nodecount() {
    return ncount;
  }
};

class Counter {
public:
  Counter(std::vector<std::string> &rows_, std::vector<int> &stack_) 
    : counter(0),rows(rows_),stack(stack_) {}
  void visit(DLink<int32_t> &dlink) {
    counter++;
    if (true) {
      int ssize = stack.size();
      std::vector<int> options;
      for (int i = 0; i < ssize; i++) {
        int opt = dlink.getoption(stack[i]);
        options.push_back(opt);
      }
      std::sort(options.begin(),options.end());
      int N = dlink.nitems();
      std::string config(N,'0');
      for (int i = 0; i < ssize; i++) {
        std::string &row = rows[options[i]];
        for (int j = 0; j < N; j++) {
          if (row[j] == '1') {
            assert(config[j] == '0');
            int k = i+1; // Reserve zero for "unoccupied"
            config[j] = k < 10 ? k+'0' : k-10+'a';
          }
        }
      }
      printf("%s\n",config.c_str());
    }
  }

  int getcounter() { return counter; }
private:
  int counter;
  std::vector<std::string> &rows;
  std::vector<int> &stack;
};
  
int main() {
  std::vector<std::string> rows;
  std::string row;
  while(std::getline(std::cin,row)) {
    if (row.length() > 0 && row[0] != '#') {
      rows.push_back(row);
    }
  }
  int nitems = rows[0].size();
  assert(nitems > 0);
  DLink<int32_t> dlink(nitems);
  for (auto row: rows) {
    assert(int(row.size()) == nitems);
    dlink.addrow(row);
  }
  dlink.stats();
  std::vector<int> stack;
  Counter counter(rows,stack);
  dlink.rdance(counter,stack);
  fprintf(stderr, "%d solutions %ld calls\n", counter.getcounter(), dlink.nodecount());
  return 0;
}
