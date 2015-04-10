/**************************************************************************
 *
 * Copyright (C) 2015 Thorsten Falk
 *
 *        Image Analysis Lab, University of Freiburg, Germany
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 **************************************************************************/

#ifndef ATBBASICTREE_HH
#define ATBBASICTREE_HH

#ifdef HAVE_CONFIG_H
#include <config.hh>
#endif

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
