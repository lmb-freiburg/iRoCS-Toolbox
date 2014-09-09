/**************************************************************************
**       Title: Basic tree structure
**    $RCSfile: ATBBasicTree.hh,v $
**   $Revision: 4804 $ $Name:  $
**       $Date: 2011-10-15 16:15:22 +0200 (Sat, 15 Oct 2011) $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**************************************************************************/

#ifndef ATBBASICTREE_HH
#define ATBBASICTREE_HH

#include <iostream>
#include <sstream>
#include <list>

namespace atb
{
  
  template<typename KeyT, typename ContentT>
  class BasicTreeNode 
  {

  public:

    BasicTreeNode(KeyT const &key, ContentT const &content = ContentT())
            : _key(key), _content(content), p_parent(NULL), _children(),
              p_root(this)
          {}

    ~BasicTreeNode() 
          {
            while (_children.size() > 0)
                _children.front()->reparent(p_parent);
            reparent(NULL);
          }

    KeyT const key() const 
          {
            return _key;
          }

    void setKey(const KeyT& key) 
          {
            _key = key;
          }
    
    ContentT const content() const
          {
            return _content;
          }

    void setContent(const ContentT& content) 
          {
            _content = content;
          }

    BasicTreeNode<KeyT, ContentT> const *parent() const
          {
            return p_parent;
          }

    BasicTreeNode<KeyT, ContentT> *parent()
          {
            return p_parent;
          }

    std::list<BasicTreeNode<KeyT, ContentT>*> const &children() const
          {
            return _children;
          }

    std::list<BasicTreeNode<KeyT, ContentT>*> &children()
          {
            return _children;
          }

    BasicTreeNode<KeyT, ContentT> const *root() const
          {
            return p_root;
          }
    
    BasicTreeNode<KeyT, ContentT> *root()
          {
            return p_root;
          }
    
    void reparent(BasicTreeNode<KeyT, ContentT>* parent) 
          {
            if (p_parent == parent) return;
            if (p_parent != NULL) p_parent->_removeChild(this);
            p_parent = parent;
            if (parent != NULL) parent->_addChild(this);
            _updateRoot();
          }
    
    void addChild(BasicTreeNode<KeyT, ContentT>* child) 
          {
            child->reparent(this);
          }

    void removeChild(BasicTreeNode<KeyT, ContentT>* child) 
          {
            child->reparent(NULL);
          }

  private:

    void _removeChild(BasicTreeNode<KeyT, ContentT>* child) 
          {
            _children.remove(child);
          }
    
    void _addChild(BasicTreeNode<KeyT, ContentT>* child) 
          {
            _children.push_back(child);
          }

    void _updateRoot()
          {
            if (p_parent == NULL) p_root = this;
            else p_root = p_parent->p_root;
            for (typename std::list<BasicTreeNode<KeyT, ContentT>*>::iterator
                     it = _children.begin(); it != _children.end(); ++it)
                (*it)->_updateRoot();
          }

    KeyT _key;
    ContentT _content;

    BasicTreeNode<KeyT, ContentT> *p_parent;
    std::list<BasicTreeNode<KeyT, ContentT>*> _children;

    BasicTreeNode<KeyT, ContentT> *p_root;
            
  };
  
  template<typename KeyT, typename ContentT>
  std::ostream &operator<<(
      std::ostream& os, BasicTreeNode<KeyT,ContentT> const &n)
  {
    os << n.key() << " ( ";
    typename std::list<BasicTreeNode<KeyT, ContentT>*>::const_iterator it;
    for (it = n.children().begin(); it != n.children().end(); ++it) 
    {
      os << **it << " ";
    }
    os << ")";
    return os;
  }

}

#endif
