// Tissue.h
// Copyright (c) 2004-7 David R.Cheriton, All rights reserved.

#ifndef TISSUE_H
#define TISSUE_H

#include "fwk/BaseNotifiee.h"
#include "fwk/NamedInterface.h"
#include "fwk/HashMap.h"
#include "fwk/ListRaw.h"
#include "fwk/LinkedList.h"
#include "fwk/LinkedQueue.h"
#include "fwk/Array.h"
#include "fwk/String.h"

#ifdef _MSC_VER
#pragma warning (disable:4250)
#endif

//----------------------------------------------------

class AntibodyStrength {
public:
   U8 value() const { return value_; }
   bool operator==(AntibodyStrength _opArg) const;
   bool operator!=(AntibodyStrength _opArg) const;
   bool operator<(AntibodyStrength _opArg) const;
   bool operator<=(AntibodyStrength _opArg) const;
   bool operator>(AntibodyStrength _opArg) const;
   bool operator>=(AntibodyStrength _opArg) const;
   void valueIs(U8 _value) {
      value_ = _value;
   }
   AntibodyStrength(U8 _value=0):
         value_(_value) {
      if((_value<=100)) { /* in range */ }
      else throw Fwk::RangeException("value=range()");
   }
protected:
   U8 value_;
};

//----------------------------------------------------

Fwk::Ostream & operator<<( Fwk::Ostream & s, AntibodyStrength const & val );

//----------------------------------------------------
class CellMembrane : public Fwk::NamedInterface {
public:
   typedef Fwk::Ptr<CellMembrane const> PtrConst;
   typedef Fwk::Ptr<CellMembrane> Ptr;

   enum Side {
      north_ = 0,
      south_ = 1,
      east_ = 2,
      west_ = 3,
      up_ = 4,
      down_ = 5
   };
   static inline Side north() { return north_; }
   static inline Side south() { return south_; }
   static inline Side east() { return east_; }
   static inline Side west() { return west_; }
   static inline Side up() { return up_; }
   static inline Side down() { return down_; }

   static Side SideInstance( U32 v );
   static Side SideInstance( Fwk::String );
   Side side() const { return side_; }

   const AntibodyStrength antibodyStrength() const { return antibodyStrength_; }
   void antibodyStrengthIs(AntibodyStrength _antibodyStrength);

   class NotifieeConst : public virtual Fwk::NamedInterface::NotifieeConst {
   public:
      typedef Fwk::Ptr<NotifieeConst const> PtrConst;
      typedef Fwk::Ptr<NotifieeConst> Ptr;
      Fwk::String name() const { return (notifier_?notifier_->name():Fwk::String()); }
      CellMembrane::PtrConst notifier() const { return notifier_; }
      bool isNonReferencing() const { return isNonReferencing_; }
      NotifieeConst const * lrNext() const { return lrNext_; }
      NotifieeConst * lrNext() { return lrNext_; }
      // Non-const interface =============================================
       ~NotifieeConst();
      virtual void notifierIs(const CellMembrane::PtrConst& _notifier);
      void isNonReferencingIs(bool _isNonReferencing);
      void lrNextIs(NotifieeConst * _lrNext) {
         lrNext_ = _lrNext;
      }
      static NotifieeConst::Ptr NotifieeConstIs() {
         Ptr m = new NotifieeConst();
         m->referencesDec(1);
         // decr. refer count to compensate for initial val of 1
         return m;
      }
      // Constructors ====================================================
   protected:
      CellMembrane::PtrConst notifier_;
      bool isNonReferencing_;
      NotifieeConst * lrNext_;
      NotifieeConst(): Fwk::NamedInterface::NotifieeConst(),
            isNonReferencing_(false),
            lrNext_(0) {}
   };

   class Notifiee : public virtual NotifieeConst, public virtual Fwk::NamedInterface::Notifiee {
   public:
      typedef Fwk::Ptr<Notifiee const> PtrConst;
      typedef Fwk::Ptr<Notifiee> Ptr;
      CellMembrane::PtrConst notifier() const { return NotifieeConst::notifier(); }
      CellMembrane::Ptr notifier() { return const_cast<CellMembrane *>(NotifieeConst::notifier().ptr()); }
      // Non-const interface =============================================
      static Notifiee::Ptr NotifieeIs() {
         Ptr m = new Notifiee();
         m->referencesDec(1);
         // decr. refer count to compensate for initial val of 1
         return m;
      }
      // Constructors ====================================================
   protected:
      Notifiee(): Fwk::NamedInterface::Notifiee() {}
   };

   virtual Fwk::String attributeString( Fwk::RootNotifiee::AttributeId ) const;
   typedef Fwk::ListRaw<NotifieeConst> NotifieeList;
   typedef NotifieeList::IteratorConst NotifieeIteratorConst;
   NotifieeIteratorConst notifieeIterConst() const { return notifiee_.iterator(); }
   U32 notifiees() const { return notifiee_.members(); }
   // Non-const interface =============================================
    ~CellMembrane();
   static CellMembrane::Ptr CellMembraneNew(Fwk::String _name,Side _side) {
      Ptr m = new CellMembrane(_name,_side);
      m->referencesDec(1);
      // decr. refer count to compensate for initial val of 1
      return m;
   }
   typedef NotifieeList::Iterator NotifieeIterator;
   NotifieeIterator notifieeIter() { return notifiee_.iterator(); }
   // Constructors ====================================================
protected:
   CellMembrane( const CellMembrane& );
   Side side_;
   void sideIs(Side _side);
   AntibodyStrength antibodyStrength_;
   CellMembrane(Fwk::String _name, Side _side);
   void newNotifiee( CellMembrane::NotifieeConst * n ) const {
      CellMembrane* me = const_cast<CellMembrane*>(this);
      me->notifiee_.newMember(n);
   }
   void deleteNotifiee( CellMembrane::NotifieeConst * n ) const {
      CellMembrane* me = const_cast<CellMembrane*>(this);
      me->notifiee_.deleteMember(n);
   }
   NotifieeList notifiee_;
   void onZeroReferences();
};

class Tissue;

class Cell : public Fwk::NamedInterface {
public:
   typedef Fwk::Ptr<Cell const> PtrConst;
   typedef Fwk::Ptr<Cell> Ptr;

   struct Coordinates {
      int x, y, z;
      Fwk::String name() const;
      bool operator==(const Coordinates& other) const
      { return x == other.x && y == other.y && z == other.z; }
      bool operator!=(const Coordinates& other) const
      { return x != other.x || y != other.y || z != other.z; }
   };
   Coordinates location() const { return location_; }

   enum CellType {
      tCell_ = 0,
      helperCell_ = 1,
      cytotoxicCell_ = 2,
      otherCell_ = 3
    };
   CellType cellType() const {return cellType_; }

   static inline CellType tCell() { return tCell_; }
   static inline CellType helperCell() { return helperCell_; }
   static inline CellType cytotoxicCell() { return cytotoxicCell_; }
   static inline CellType otherCell() { return otherCell_; }
   enum HealthId {
      healthy_ = 1,
      infected_
   };
   static inline HealthId healthy() { return healthy_; }
   static inline HealthId infected() { return infected_; }

   static HealthId HealthIdInstance( U32 v );
   static HealthId HealthIdInstance( Fwk::String );
   HealthId health() const { return health_; }
   void healthIs(HealthId _health);
   CellMembrane::PtrConst membrane(CellMembrane::Side _side) const {
      return membrane_[_side];
   }
   CellMembrane::Ptr membrane(CellMembrane::Side _side) {
      return membrane_[_side];
   }
   typedef Fwk::ArrayIteratorConst< CellMembrane::Ptr,CellMembrane::Side > MembraneIteratorConst;
   MembraneIteratorConst membraneIterConst() const {
      return MembraneIteratorConst( membrane_ ); }
   MembraneIteratorConst membraneIterConst(CellMembrane::Side _start) const {
      return MembraneIteratorConst( membrane_, _start ); }
   U32 membranes() const {
      MembraneIteratorConst i=membraneIterConst();
      U32 result = 0;
      while( i ) { ++result; ++i; }
      return result;
   }

   class NotifieeConst : public virtual Fwk::NamedInterface::NotifieeConst {
   public:
      typedef Fwk::Ptr<NotifieeConst const> PtrConst;
      typedef Fwk::Ptr<NotifieeConst> Ptr;
      Fwk::String name() const { return notifier_->name(); }
      Cell::PtrConst notifier() const { return notifier_; }
      bool isNonReferencing() const { return isNonReferencing_; }
      static const AttributeId membrane__ = AttributeId(Fwk::NamedInterface::NotifieeConst::tacNextAttributeId__);
      static const AttributeId tissue__ = AttributeId(membrane__+1);
      static const AttributeId tacNextAttributeId__ = AttributeId(membrane__+2);
      CellMembrane::Side tacKeyForMembrane() const { return tacKeyForMembrane_; }
      U8 tacMembraneChanges() const { return tacMembraneChanges_; }
      NotifieeConst const * lrNext() const { return lrNext_; }
      NotifieeConst * lrNext() { return lrNext_; }
      // Non-const interface =============================================
       ~NotifieeConst();
      virtual void notifierIs(const Cell::PtrConst& _notifier);
      void isNonReferencingIs(bool _isNonReferencing);
      virtual void onMembrane( CellMembrane::Side _side ) {}
      void onMembrane() {}
      void tacKeyForMembraneIs(CellMembrane::Side _tacKeyForMembrane) {
         tacKeyForMembrane_ = _tacKeyForMembrane;
      }
      void tacMembraneChangesIs(U8 _tacMembraneChanges) {
         tacMembraneChanges_ = _tacMembraneChanges;
      }
      virtual void onTissue() {}
      void lrNextIs(NotifieeConst * _lrNext) {
         lrNext_ = _lrNext;
      }
      static NotifieeConst::Ptr NotifieeConstIs() {
         Ptr m = new NotifieeConst();
         m->referencesDec(1);
         // decr. refer count to compensate for initial val of 1
         return m;
      }
      // Constructors ====================================================
   protected:
      Cell::PtrConst notifier_;

      bool isNonReferencing_;
      CellMembrane::Side tacKeyForMembrane_;
      U8 tacMembraneChanges_;
      NotifieeConst * lrNext_;
      NotifieeConst(): Fwk::NamedInterface::NotifieeConst(),
            isNonReferencing_(false),
            tacKeyForMembrane_(CellMembrane::north_),
            tacMembraneChanges_(0),
            lrNext_(0) { }
   };
   class Notifiee : public virtual NotifieeConst, public virtual Fwk::NamedInterface::Notifiee {
   public:
      typedef Fwk::Ptr<Notifiee const> PtrConst;
      typedef Fwk::Ptr<Notifiee> Ptr;
      Cell::PtrConst notifier() const { return NotifieeConst::notifier(); }
      Cell::Ptr notifier() { return const_cast<Cell *>(NotifieeConst::notifier().ptr()); }
      // Non-const interface =============================================
      static Notifiee::Ptr NotifieeIs() {
         Ptr m = new Notifiee();
         m->referencesDec(1);
         // decr. refer count to compensate for initial val of 1
         return m;
      }
      // Constructors ====================================================
   protected:
      Notifiee(): Fwk::NamedInterface::Notifiee() { }
   };
   Tissue const * tissue() const { return tissue_; }
   Tissue * tissue() { return tissue_; }
   Fwk::String fwkKey() const { return name(); }
   Cell const * fwkHmNext() const { return fwkHmNext_.ptr(); }
   Cell * fwkHmNext() { return fwkHmNext_.ptr(); }
   Cell const * fwkPtr() const { return this; }
   Cell * fwkPtr() { return this; }
   Cell::PtrConst fwkValue() const { return this; }
   Cell::Ptr fwkValue() { return this; }
   virtual Fwk::String attributeString( Fwk::RootNotifiee::AttributeId ) const;
   typedef Fwk::ListRaw<NotifieeConst> NotifieeList;
   typedef NotifieeList::IteratorConst NotifieeIteratorConst;
   NotifieeIteratorConst notifieeIterConst() const { return notifiee_.iterator(); }
   U32 notifiees() const { return notifiee_.members(); }
   // Non-const interface =============================================
    ~Cell();
   typedef Fwk::ArrayIterator< CellMembrane::Ptr,CellMembrane::Side > MembraneIterator;
   MembraneIterator membraneIter() {
      return MembraneIterator( membrane_ ); }
   MembraneIterator membraneIter(CellMembrane::Side _start) {
      return MembraneIterator( membrane_, _start ); }
   CellMembrane::Ptr membraneDel(CellMembrane::Side _side);
   CellMembrane::Ptr membraneNew(Fwk::String _name,CellMembrane::Side _side);
   void fwkHmNextIs(Cell * _fwkHmNext) const {
      fwkHmNext_ = _fwkHmNext;
   }
   static Cell::Ptr CellNew(Cell::Coordinates _loc, Tissue * _tissue, CellType _type) {
     Ptr m = new Cell(_loc, _tissue, _type);
      m->referencesDec(1);
      // decr. refer count to compensate for initial val of 1
      return m;
   }
   typedef NotifieeList::Iterator NotifieeIterator;
   NotifieeIterator notifieeIter() { return notifiee_.iterator(); }
   // Constructors ====================================================
protected:
   Cell( const Cell& );
   // Undefined copy constructor to preclude copy.
   Coordinates location_;
   HealthId health_;
   CellMembrane::Ptr newMembrane( Fwk::String _name, CellMembrane::Side _side );
   Fwk::Array<CellMembrane::Ptr,6, CellMembrane::Side> membrane_;
   Tissue* tissue_;
   CellType cellType_;

   mutable Cell::Ptr fwkHmNext_;
   friend class Tissue;
   void tissueIs(Tissue * _tissue);
   Cell(Coordinates _loc, Tissue * _tissue, CellType _type);
   void newNotifiee( Cell::NotifieeConst * n ) const {
      Cell* me = const_cast<Cell*>(this);
      me->notifiee_.newMember(n);
   }
   void deleteNotifiee( Cell::NotifieeConst * n ) const {
      Cell* me = const_cast<Cell*>(this);
      me->notifiee_.deleteMember(n);
   }
   NotifieeList notifiee_;
   void onZeroReferences();
};






class Tissue : public Fwk::NamedInterface {
public:
   typedef Fwk::Ptr<Tissue const> PtrConst;
   typedef Fwk::Ptr<Tissue> Ptr;
   Cell::PtrConst cell(Cell::Coordinates _loc) const {
      return cell_[_loc.name()];
   }
   Cell::Ptr cell(Cell::Coordinates _loc) {
      return cell_[_loc.name()];
   }
   typedef Fwk::HashMap< Cell, Fwk::String, Cell, Cell::PtrConst, Cell::Ptr > CellMap;


   U32 cells() const { return cell_.members(); }
   U32 cellVersion() const { return cell_.version(); }
   typedef CellMap::IteratorConst CellIteratorConst;
   CellIteratorConst cellIterConst() const { return cell_.iterator(); }
   CellIteratorConst cellIterConst( Cell::Coordinates _loc ) const {
      return cell_.iterator( _loc.name() ); }
   typedef CellMap::Iterator CellIterator;
   CellIterator cellIter() { return cell_.iterator(); }
   CellIterator cellIter( Cell::Coordinates _loc ) {
      return cell_.iterator( _loc.name() ); }


   class NotifieeConst : public virtual Fwk::NamedInterface::NotifieeConst {
   public:
      typedef Fwk::Ptr<NotifieeConst const> PtrConst;
      typedef Fwk::Ptr<NotifieeConst> Ptr;
      Fwk::String name() const { return notifier_->name(); }
      Tissue::PtrConst notifier() const { return notifier_; }
      bool isNonReferencing() const { return isNonReferencing_; }
      static const AttributeId cell__ = AttributeId(Fwk::NamedInterface::NotifieeConst::tacNextAttributeId__);
      static const AttributeId tacNextAttributeId__ = AttributeId(cell__+1);
      Fwk::String tacKeyForCell() const { return tacKeyForCell_; }
      U8 tacCellChanges() const { return tacCellChanges_; }
      NotifieeConst const * lrNext() const { return lrNext_; }
      NotifieeConst * lrNext() { return lrNext_; }
      // Non-const interface =============================================
       ~NotifieeConst();
      virtual void notifierIs(const Tissue::PtrConst& _notifier);
      void isNonReferencingIs(bool _isNonReferencing);
      virtual void onCellNew( Cell::Ptr ) {}
      virtual void onCellDel( Fwk::String _name ) {}
      virtual void onCellDel( Cell::Ptr ) {}


void onCell() {}
      void tacKeyForCellIs(Fwk::String _tacKeyForCell) {
         tacKeyForCell_ = _tacKeyForCell;
      }
      void tacCellChangesIs(U8 _tacCellChanges) {
         tacCellChanges_ = _tacCellChanges;
      }
      void lrNextIs(NotifieeConst * _lrNext) {
         lrNext_ = _lrNext;
      }
      static NotifieeConst::Ptr NotifieeConstIs() {
         Ptr m = new NotifieeConst();
         m->referencesDec(1);
         // decr. refer count to compensate for initial val of 1
         return m;
      }
      // Constructors ====================================================
   protected:
      Tissue::PtrConst notifier_;
      bool isNonReferencing_;
      Fwk::String tacKeyForCell_;
      U8 tacCellChanges_;
      NotifieeConst * lrNext_;
      NotifieeConst(): Fwk::NamedInterface::NotifieeConst(),
            isNonReferencing_(false),
            tacCellChanges_(0),
            lrNext_(0) {}
   };

   class Notifiee : public virtual NotifieeConst, public virtual Fwk::NamedInterface::Notifiee {
   public:
      typedef Fwk::Ptr<Notifiee const> PtrConst;
      typedef Fwk::Ptr<Notifiee> Ptr;
      Tissue::PtrConst notifier() const { return NotifieeConst::notifier(); }
      Tissue::Ptr notifier() { return const_cast<Tissue *>(NotifieeConst::notifier().ptr()); }
      // Non-const interface =============================================
      static Notifiee::Ptr NotifieeIs() {
         Ptr m = new Notifiee();
         m->referencesDec(1);
         // decr. refer count to compensate for initial val of 1
         return m;
      }
      // Constructors ====================================================
   protected:
      Notifiee(): Fwk::NamedInterface::Notifiee() {}
   };
   virtual Fwk::String attributeString( Fwk::RootNotifiee::AttributeId ) const;
   typedef Fwk::ListRaw<NotifieeConst> NotifieeList;
   typedef NotifieeList::IteratorConst NotifieeIteratorConst;
   NotifieeIteratorConst notifieeIterConst() const { return notifiee_.iterator(); }
   U32 notifiees() const { return notifiee_.members(); }
   // Non-const interface =============================================
    ~Tissue();
   Cell::Ptr cellDel(Fwk::String _name);
   Cell::Ptr cellIs(Cell::Ptr cell);
   static Tissue::Ptr TissueNew(Fwk::String _name) {
      Ptr m = new Tissue(_name);
      m->referencesDec(1);
      // decr. refer count to compensate for initial val of 1
      return m;
   }
   typedef NotifieeList::Iterator NotifieeIterator;
   NotifieeIterator notifieeIter() { return notifiee_.iterator(); }
   // Constructors ====================================================
protected:
   Tissue( const Tissue& );
   // Cell::Ptr newCell( Cell::Coordinates _loc );
   CellMap   cell_;
   explicit Tissue(Fwk::String _name);
   void newNotifiee( Tissue::NotifieeConst * n ) const {
      Tissue* me = const_cast<Tissue*>(this);
      me->notifiee_.newMember(n);
   }
   void deleteNotifiee( Tissue::NotifieeConst * n ) const {
      Tissue* me = const_cast<Tissue*>(this);
      me->notifiee_.deleteMember(n);
   }
   NotifieeList notifiee_;
   void onZeroReferences();
};


class TCell : public Cell {
public:
   typedef Fwk::Ptr<TCell const> PtrConst;
   typedef Fwk::Ptr<TCell> Ptr;

   class NotifieeConst : public virtual Cell::NotifieeConst {
   public:
      typedef Fwk::Ptr<NotifieeConst const> PtrConst;
      typedef Fwk::Ptr<NotifieeConst> Ptr;
      TCell::PtrConst notifier() const { return dynamic_cast<TCell const *>(Cell::NotifieeConst::notifier().ptr()); }
      static const AttributeId tacNextAttributeId__ = AttributeId(Cell::NotifieeConst::nextAttributeNumber__);
      NotifieeConst const * lrNext() const { return lrNext_; }
      NotifieeConst * lrNext() { return lrNext_; }
      // Non-const interface =============================================
      void lrNextIs(NotifieeConst * _lrNext) {
         lrNext_ = _lrNext;
      }
      static NotifieeConst::Ptr NotifieeConstIs() {
         Ptr m = new NotifieeConst();
         m->referencesDec(1);
         // decr. refer count to compensate for initial val of 1
         return m;
      }
      // Constructors ====================================================
   protected:
      NotifieeConst * lrNext_;
      NotifieeConst(): Cell::NotifieeConst(),
            lrNext_(0) {}
   };

   class Notifiee : public virtual NotifieeConst, public virtual Cell::Notifiee {
   public:
      typedef Fwk::Ptr<Notifiee const> PtrConst;
      typedef Fwk::Ptr<Notifiee> Ptr;
      TCell::PtrConst notifier() const { return dynamic_cast<TCell const *>(Cell::NotifieeConst::notifier().ptr()); }
      TCell::Ptr notifier() { return dynamic_cast<TCell *>(const_cast<Cell *>(Cell::NotifieeConst::notifier().ptr())); }
      // Non-const interface =============================================
      static Notifiee::Ptr NotifieeIs() {
         Ptr m = new Notifiee();
         m->referencesDec(1);
         // decr. refer count to compensate for initial val of 1
         return m;
      }
      // Constructors ====================================================
   protected:
      Notifiee(): Cell::Notifiee() {}
   };
   Fwk::String fwkKey() const { return name(); }
   virtual Fwk::String attributeString( Fwk::RootNotifiee::AttributeId ) const;
   // Non-const interface =============================================
   static TCell::Ptr TCellIs(Coordinates _loc, Tissue * _tissue, Cell::CellType _type) {
      Ptr m = new TCell(_loc,_tissue, _type);
      m->referencesDec(1);
      // decr. refer count to compensate for initial val of 1
      return m;
   }
   // Constructors ====================================================
protected:
   TCell( const TCell& );
   CellMembrane::Ptr newMembrane( Fwk::String _name, CellMembrane::Side _side );
   TCell(Coordinates _loc, Tissue * _tissue, Cell::CellType);
};

class CytotoxicCell : public TCell {
public:
   typedef Fwk::Ptr<CytotoxicCell const> PtrConst;
   typedef Fwk::Ptr<CytotoxicCell> Ptr;

   class NotifieeConst : public virtual TCell::NotifieeConst {
   public:
      typedef Fwk::Ptr<NotifieeConst const> PtrConst;
      typedef Fwk::Ptr<NotifieeConst> Ptr;
      CytotoxicCell::PtrConst notifier() const { return dynamic_cast<CytotoxicCell const *>(Cell::NotifieeConst::notifier().ptr()); }
	  static const AttributeId tacNextAttributeId__ = AttributeId(TCell::NotifieeConst::nextAttributeNumber__);
      NotifieeConst const * lrNext() const { return lrNext_; }
      NotifieeConst * lrNext() { return lrNext_; }
      // Non-const interface =============================================
      void lrNextIs(NotifieeConst * _lrNext) {
         lrNext_ = _lrNext;
      }
      static NotifieeConst::Ptr NotifieeConstIs() {
         Ptr m = new NotifieeConst();
         m->referencesDec(1);
         // decr. refer count to compensate for initial val of 1
         return m;
      }
      // Constructors ====================================================
   protected:
      NotifieeConst * lrNext_;
      NotifieeConst(): TCell::NotifieeConst(),
            lrNext_(0) {}
   };

   class Notifiee : public virtual NotifieeConst, public virtual TCell::Notifiee {
   public:
      typedef Fwk::Ptr<Notifiee const> PtrConst;
      typedef Fwk::Ptr<Notifiee> Ptr;
      CytotoxicCell::PtrConst notifier() const { return dynamic_cast<CytotoxicCell const *>(Cell::NotifieeConst::notifier().ptr()); }
      CytotoxicCell::Ptr notifier() { return dynamic_cast<CytotoxicCell *>(const_cast<Cell *>(Cell::NotifieeConst::notifier().ptr())); }
      // Non-const interface =============================================
      static Notifiee::Ptr NotifieeIs() {
         Ptr m = new Notifiee();
         m->referencesDec(1);
         // decr. refer count to compensate for initial val of 1
         return m;
      }
      // Constructors ====================================================
   protected:
      Notifiee(): TCell::Notifiee() {}
   };
   Fwk::String fwkKey() const { return name(); }
   virtual Fwk::String attributeString( Fwk::RootNotifiee::AttributeId ) const;
   // Non-const interface =============================================
   static CytotoxicCell::Ptr CytotoxicCellIs(Coordinates _loc, Tissue * _tissue) {
      Ptr m = new CytotoxicCell(_loc,_tissue);
      m->referencesDec(1);
      // decr. refer count to compensate for initial val of 1
      return m;
   }

   // Constructors ====================================================
protected:
   CytotoxicCell( const CytotoxicCell& );
   CellMembrane::Ptr newMembrane( Fwk::String _name, CellMembrane::Side _side );
   CytotoxicCell(Coordinates _loc, Tissue * _tissue);
};

class HelperCell : public TCell {
public:
   typedef Fwk::Ptr<HelperCell const> PtrConst;
   typedef Fwk::Ptr<HelperCell> Ptr;

   class NotifieeConst : public virtual TCell::NotifieeConst {
   public:
      typedef Fwk::Ptr<NotifieeConst const> PtrConst;
      typedef Fwk::Ptr<NotifieeConst> Ptr;
      HelperCell::PtrConst notifier() const { return dynamic_cast<HelperCell const *>(Cell::NotifieeConst::notifier().ptr()); }
      static const AttributeId tacNextAttributeId__ = AttributeId(TCell::NotifieeConst::nextAttributeNumber__);
      NotifieeConst const * lrNext() const { return lrNext_; }
      NotifieeConst * lrNext() { return lrNext_; }
      // Non-const interface =============================================
      void lrNextIs(NotifieeConst * _lrNext) {
         lrNext_ = _lrNext;
      }
      static NotifieeConst::Ptr NotifieeConstIs() {
         Ptr m = new NotifieeConst();
         m->referencesDec(1);
         // decr. refer count to compensate for initial val of 1
         return m;
      }
      // Constructors ====================================================
   protected:
      NotifieeConst * lrNext_;
      NotifieeConst(): TCell::NotifieeConst(),
            lrNext_(0) {}
   };

   class Notifiee : public virtual NotifieeConst, public virtual TCell::Notifiee {
   public:
      typedef Fwk::Ptr<Notifiee const> PtrConst;
      typedef Fwk::Ptr<Notifiee> Ptr;
      HelperCell::PtrConst notifier() const { return dynamic_cast<HelperCell const *>(Cell::NotifieeConst::notifier().ptr()); }
      HelperCell::Ptr notifier() { return dynamic_cast<HelperCell *>(const_cast<Cell *>(Cell::NotifieeConst::notifier().ptr())); }
      // Non-const interface =============================================
      static Notifiee::Ptr NotifieeIs() {
         Ptr m = new Notifiee();
         m->referencesDec(1);
         // decr. refer count to compensate for initial val of 1
         return m;
      }
      // Constructors ====================================================
   protected:
      Notifiee(): TCell::Notifiee() {}
   };
   Fwk::String fwkKey() const { return name(); }
   virtual Fwk::String attributeString( Fwk::RootNotifiee::AttributeId ) const;
   // Non-const interface =============================================
   static HelperCell::Ptr HelperCellIs(Coordinates _loc, Tissue * _tissue) {
     Ptr m = new HelperCell(_loc, _tissue);
      m->referencesDec(1);
      // decr. refer count to compensate for initial val of 1
      return m;
   }
   // Constructors ====================================================
protected:
   HelperCell( const HelperCell& );
   CellMembrane::Ptr newMembrane( Fwk::String _name, CellMembrane::Side _side );
   HelperCell(Coordinates _loc, Tissue * _tissue);
};


Tissue::Ptr tissueFactory( Fwk::String _name );
Fwk::String valueToStrep( CellMembrane::Side s );


#endif
