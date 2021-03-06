#define INC(c) ((c) == K - 1 ? 0 : (c) + 1)
template <int K> struct kd_tree {
  struct pt {
    double coord[K];
    pt() {}
    pt(double c[K]) { REP(i,K) coord[i] = c[i]; }
    double dist(const pt &other) const {
      double sum = 0.0;
      REP(i,K) sum += pow(coord[i]-other.coord[i],2);
      return sqrt(sum); } };
  struct cmp {
    int c;
    cmp(int _c) : c(_c) {}
    bool operator ()(const pt &a, const pt &b) {
      for (int i = 0, cc; i <= K; i++) {
        cc = i == 0 ? c : i - 1;
        if (abs(a.coord[cc] - b.coord[cc]) > EPS)
          return a.coord[cc] < b.coord[cc];
      }
      return false; } };
  struct bb {
    pt from, to;
    bb(pt _from, pt _to) : from(_from), to(_to) {}
    double dist(const pt &p) {
      double sum = 0.0;
      REP(i,K) {
        if (p.coord[i] < from.coord[i])
          sum += pow(from.coord[i] - p.coord[i], 2.0);
        else if (p.coord[i] > to.coord[i])
          sum += pow(p.coord[i] - to.coord[i], 2.0);
      }
      return sqrt(sum); }
    bb bound(double l, int c, bool left) {
      pt nf(from.coord), nt(to.coord);
      if (left) nt.coord[c] = min(nt.coord[c], l);
      else nf.coord[c] = max(nf.coord[c], l);
      return bb(nf, nt); } };
  struct node {
    pt p; node *l, *r;
    node(pt _p, node *_l, node *_r)
      : p(_p), l(_l), r(_r) {  } };
  node *root;

  // kd_tree() : root(NULL) { }
  kd_tree(vector<pt> pts) {
    root = construct(pts, 0, size(pts) - 1, 0); }
  node* construct(vector<pt> &pts, int fr, int to, int c) {
    if (fr > to) return NULL;
    int mid = fr + (to-fr) / 2;
    nth_element(pts.begin() + fr, pts.begin() + mid,
          pts.begin() + to + 1, cmp(c));
    return new node(pts[mid],
            construct(pts, fr, mid - 1, INC(c)),
            construct(pts, mid + 1, to, INC(c))); }
  bool contains(const pt &p){ return _con(p,root,0);}
  bool _con(const pt &p, node *n, int c) {
    if (!n) return false;
    if (cmp(c)(p, n->p)) return _con(p,n->l,INC(c));
    if (cmp(c)(n->p, p)) return _con(p,n->r,INC(c));
    return true; }
  void insert(const pt &p) { _ins(p, root, 0); }
  void _ins(const pt &p, node* &n, int c) {
    if (!n) n = new node(p, NULL, NULL);
    else if (cmp(c)(p, n->p)) _ins(p, n->l, INC(c));
    else if (cmp(c)(n->p, p)) _ins(p, n->r, INC(c));
  }
  void clear() { _clr(root); root = NULL; }
  void _clr(node *n) {
    if (n) _clr(n->l), _clr(n->r), delete n; }
  pt nearest_neighbour(const pt &p, bool same=true) {
    assert(root);
    double mn = INFINITY, cs[K];
    REP(i,K) cs[i] = -INFINITY;
    pt from(cs);
    REP(i,K) cs[i] = INFINITY;
    pt to(cs);
    return _nn(p, root, bb(from, to), mn, 0, same).x;
  }
  pair<pt, bool> _nn(const pt &p, node *n, bb b,
      double &mn, int c, bool same) {
    if (!n || b.dist(p) > mn)
      return make_pair(pt(), false);
    bool found = same || p.dist(n->p) > EPS,
         l1 = true, l2 = false;
    pt resp = n->p;
    if (found) mn = min(mn, p.dist(resp));
    node *n1 = n->l, *n2 = n->r;
    REP(i,2) {
      if (i == 1 || cmp(c)(n->p, p))
        swap(n1, n2), swap(l1, l2);
      auto res = _nn(p, n1, b.bound(n->p.coord[c], c, l1), mn, INC(c), same);
      if (res.y && (!found || p.dist(res.x) < p.dist(resp)))
        resp = res.x, found = true;
    }
    return make_pair(resp, found); } };
