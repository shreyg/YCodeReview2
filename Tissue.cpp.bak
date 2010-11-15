// HivTissue.cpp implementation
// Copyright (c) 2004-7 David R. Cheriton, All rights reserved.

#include "fwk/String.h"
#include "Tissue.h"
//----------| AntibodyStrength Implementation |------------//

bool
AntibodyStrength::operator==(AntibodyStrength _tacOpArg) const { return (value_==_tacOpArg.value_); }

bool
AntibodyStrength::operator!=(AntibodyStrength _tacOpArg) const { return !(*this==_tacOpArg); }

bool
AntibodyStrength::operator<(AntibodyStrength _tacOpArg) const { return (value_<_tacOpArg.value_); }

bool
AntibodyStrength::operator<=(AntibodyStrength _tacOpArg) const { return !(_tacOpArg<*this); }

bool
AntibodyStrength::operator>(AntibodyStrength _tacOpArg) const { return !(*this<=_tacOpArg); }

bool
AntibodyStrength::operator>=(AntibodyStrength _tacOpArg) const { return !(*this<_tacOpArg); }

Fwk::Ostream& operator<<( Fwk::Ostream & s, AntibodyStrength const & val ) {
   s << (U32)val.value(); // Need this so that ostream doesn't try to print the value as a character
   return s;
}

/*
Fwk::String valueToStrep( AntibodyStrength const & val ) {
  Fwk::String str;
  if( val.value_ == 100 )
	str += "100";
  else
  {
	str += val.value_/10 + '0';
	str += val.value_%10 + '0';
  }
  return str;
}
*/

//----------| CellMembrane Implementation |------------//

void
CellMembrane::onZeroReferences() {
  retry:
   U32 ver = notifiee_.version();
   if(notifiees()) for( NotifieeIterator n = notifieeIter(); n.ptr(); ++n ) try {
      n->isNonReferencingIs(false);
      n->onDelete();
      if(ver != notifiee_.version()) goto retry;
      // If notification modified the list, then restart the iteration
   }
   catch(...) { n->onNotificationException(Fwk::RootNotifiee::references__); }
   if(notifiees()) {
      assert(references());
      return;
   }
   // handle case of notifiee being notified when refcount goes to zero
   // vs. autodelete by the notifiee destructor
   // Notifiees that do not hold a smart pointer to the notifier
   // (like proxy instances of TacEntityLog) must remove themselves
   // from the notifiee list.

   delete this;
}

//----------| NotifieeConst Implementation |------------//


CellMembrane::NotifieeConst::~NotifieeConst() {
   if(notifier_) {
      notifier_->deleteNotifiee(this);
   }
   if(notifier_&&isNonReferencing()) notifier_->newRef();
}

void
CellMembrane::NotifieeConst::notifierIs(const CellMembrane::PtrConst& _notifier) {
   CellMembrane::Ptr notifierSave(const_cast<CellMembrane *>(notifier_.ptr()));
   if(_notifier==notifier_) return;
   notifier_ = _notifier;
   if(notifierSave) {
      notifierSave->deleteNotifiee(this);
   }
   if(_notifier) {
      _notifier->newNotifiee(this);
   }
   if(isNonReferencing_) {
      if(notifierSave) notifierSave->newRef();
      if(notifier_) notifier_->deleteRef();
   }
}

void
CellMembrane::NotifieeConst::isNonReferencingIs(bool _isNonReferencing){
   if(_isNonReferencing==isNonReferencing_) return;
   isNonReferencing_ = _isNonReferencing;
   if(notifier_) {
      if(_isNonReferencing) notifier_->deleteRef();
      else notifier_->newRef();
   }
   }

//----------| Notifiee Implementation |------------//

Fwk::String
CellMembrane::attributeString( Fwk::RootNotifiee::AttributeId a ) const {
   Fwk::String str = "unknown";
   switch(a) {
      case NotifieeConst::version__: str = "version"; break;
      case NotifieeConst::notificationException__: str = "notificationException"; break;
      default: str = Fwk::RootNotifiee::attributeString(a);
   }
   return str;
}


CellMembrane::~CellMembrane() {
}

CellMembrane::Side CellMembrane::SideInstance( U32 v ) {
   switch( v ) {
      case north_ : return north_;
      case south_ : return south_;
      case east_ : return east_;
      case west_ : return west_;
      case up_ : return up_;
      case down_ : return down_;
      default : throw Fwk::RangeException( "CellMembrane::Side" );
   }
}

Fwk::String stringValue( CellMembrane::Side e )  {
   switch( e ) {
      case CellMembrane::north_ : return Fwk::String("north");
      case CellMembrane::south_ : return Fwk::String("south");
      case CellMembrane::east_ : return Fwk::String("east");
      case CellMembrane::west_ : return Fwk::String("west");
      case CellMembrane::up_ : return Fwk::String("up");
      case CellMembrane::down_ : return Fwk::String("down");
      default : {
	      Fwk::StringBuf err;
	      err << "CellMembrane::Side(" << e << ")";
	      return err;
      }
   }
}

CellMembrane::Side CellMembrane::SideInstance( Fwk::String str ) {
   if( 0 ) {
   } else if( str == "north" ) { return north_;
   } else if( str == "south" ) { return south_;
   } else if( str == "east" ) { return east_;
   } else if( str == "west" ) { return west_;
   } else if( str == "up" ) { return up_;
   } else if( str == "down" ) { return down_;
   } else {
      throw Fwk::RangeException( "Side" );
   }
}

void
CellMembrane::sideIs(CellMembrane::Side _side){
   side_ = _side;
   }

void
CellMembrane::antibodyStrengthIs(AntibodyStrength _antibodyStrength)
{
	antibodyStrength_ = _antibodyStrength;
}

CellMembrane::CellMembrane(Fwk::String _name, Side _side): Fwk::NamedInterface(_name),side_(_side),antibodyStrength_(0) {
}

//----------| Cell Implementation |------------//

Fwk::String
Cell::Coordinates::name() const
{
	Fwk::StringBuf n;
	n << '(' << x << ',' << y << ',' << z << ')';
	return n;
}

void
Cell::onZeroReferences() {
  retry:
   U32 ver = notifiee_.version();
   if(notifiees()) for( NotifieeIterator n = notifieeIter(); n.ptr(); ++n ) try {
      n->isNonReferencingIs(false);
      n->onDelete();
      if(ver != notifiee_.version()) goto retry;
      // If notification modified the list, then restart the iteration
   }
   catch(...) { n->onNotificationException(Fwk::RootNotifiee::references__); }
   if(notifiees()) {
      assert(references());
      return;
   }
   // handle case of notifiee being notified when refcount goes to zero
   // vs. autodelete by the notifiee destructor
   // Notifiees that do not hold a smart pointer to the notifier
   // (like proxy instances of TacEntityLog) must remove themselves
   // from the notifiee list.

   delete this;
}

//----------| NotifieeConst Implementation |------------//


Cell::NotifieeConst::~NotifieeConst() {
   if(notifier_) {
      notifier_->deleteNotifiee(this);
   }
   if(notifier_&&isNonReferencing()) notifier_->newRef();
}

void
Cell::NotifieeConst::notifierIs(const Cell::PtrConst& _notifier) {
   Cell::Ptr notifierSave(const_cast<Cell *>(notifier_.ptr()));
   if(_notifier==notifier_) return;
   notifier_ = _notifier;
   if(notifierSave) {
      notifierSave->deleteNotifiee(this);
   }
   if(_notifier) {
      _notifier->newNotifiee(this);
   }
   if(isNonReferencing_) {
      if(notifierSave) notifierSave->newRef();
      if(notifier_) notifier_->deleteRef();
   }
}

void
Cell::NotifieeConst::isNonReferencingIs(bool _isNonReferencing){
   if(_isNonReferencing==isNonReferencing_) return;
   isNonReferencing_ = _isNonReferencing;
   if(notifier_) {
      if(_isNonReferencing) notifier_->deleteRef();
      else notifier_->newRef();
   }
   }

//----------| Notifiee Implementation |------------//

Fwk::String
Cell::attributeString( Fwk::RootNotifiee::AttributeId a ) const {
   Fwk::String str = "unknown";
   switch(a) {
      case NotifieeConst::version__: str = "version"; break;
      case NotifieeConst::notificationException__: str = "notificationException"; break;
      case NotifieeConst::membrane__: str = "membrane"; break;
      case NotifieeConst::tissue__: str = "tissue"; break;
      default: str = Fwk::RootNotifiee::attributeString(a); break;
   }
   return str;
}


Cell::~Cell() {
   for( U32 i=CellMembrane::north_;i<=CellMembrane::down_;++i) {
      membraneDel(CellMembrane::Side(i));
   }
}
Cell::HealthId Cell::HealthIdInstance( U32 v ) {
   switch( v ) {
      case healthy_ : return healthy_;
      case infected_ : return infected_;
      default : throw Fwk::RangeException( "Cell::HealthId" );
   }
}

Fwk::String stringValue( Cell::HealthId e )  {
   switch( e ) {
      case Cell::healthy_ : return Fwk::String("healthy");
      case Cell::infected_ : return Fwk::String("infected");
      default : {
	      Fwk::StringBuf err;
	      err << "Cell::HealthId(" << e << ")";
	      return err;
      }
   }
}

Cell::HealthId Cell::HealthIdInstance( Fwk::String str ) {
   if( 0 ) {
   } else if( str == "healthy" ) { return healthy_;
   } else if( str == "infected" ) { return infected_;
   } else {
      throw Fwk::RangeException( "HealthId" );
   }
}

void
Cell::healthIs(Cell::HealthId _health){
   health_ = _health;
   }

CellMembrane::Ptr
Cell::newMembrane( Fwk::String _name, CellMembrane::Side _side ) {
   CellMembrane::Ptr mem = CellMembrane::CellMembraneNew(_name,_side);
   membrane_[_side] = mem;
   return mem;
}

CellMembrane::Ptr
Cell::membraneDel(CellMembrane::Side _side) {
   CellMembrane::Ptr m = membrane_[_side-0];
   membrane_[_side-0] = 0;
   if(!m) return 0;
   retrymembraneDel:
   U32 ver = notifiee_.version();
   if(notifiees()) for(NotifieeIterator n=notifieeIter();n.ptr();++n) try {
      n->onMembrane(_side);
      if( ver != notifiee_.version() ) goto retrymembraneDel;
   } catch(...) { n->onNotificationException(NotifieeConst::membrane__); }
   return m;
}

CellMembrane::Ptr
Cell::membraneNew(Fwk::String _name,CellMembrane::Side _side) {
   CellMembrane::Ptr m = membrane_[_side-0];
   if(m) {
      if(m->name()!=_name) throw Fwk::NameInUseException(_name);
      return m;
   } else {
      m = newMembrane(_name,_side);
   }
   retrymembrane:
   U32 ver = notifiee_.version();
   if(notifiees()) for(NotifieeIterator n=notifieeIter();n.ptr();++n) try {
      n->onMembrane(_side);
      if( ver != notifiee_.version() ) goto retrymembrane;
   } catch(...) { n->onNotificationException(NotifieeConst::membrane__); }
   return m;
}

void
Cell::tissueIs(Tissue * _tissue){
   if(_tissue==tissue_) return;
   tissue_ = _tissue;
   retrytissue:
   U32 ver = notifiee_.version();
   if(notifiees()) for(NotifieeIterator n=notifieeIter();n.ptr();++n) try {
      n->onTissue();
      if( ver != notifiee_.version() ) goto retrytissue;
   } catch(...) { n->onNotificationException(NotifieeConst::tissue__); }
   }

Cell::Cell(Coordinates _loc, Tissue * _tissue, Cell::CellType _type):
      Fwk::NamedInterface(_loc.name()),
      location_(_loc),
      health_(healthy_),
      tissue_(_tissue),
      cellType_(_type){
}

//----------| Tissue Implementation |------------//

void
Tissue::onZeroReferences() {
  retry:
   U32 ver = notifiee_.version();
   if(notifiees()) for( NotifieeIterator n = notifieeIter(); n.ptr(); ++n ) try {
      n->isNonReferencingIs(false);
      n->onDelete();
      if(ver != notifiee_.version()) goto retry;
      // If notification modified the list, then restart the iteration
   }
   catch(...) { n->onNotificationException(Fwk::RootNotifiee::references__); }
   if(notifiees()) {
      assert(references());
      return;
   }
   // handle case of notifiee being notified when refcount goes to zero
   // vs. autodelete by the notifiee destructor
   // Notifiees that do not hold a smart pointer to the notifier
   // (like proxy instances of TacEntityLog) must remove themselves
   // from the notifiee list.
   delete this;
}

//----------| NotifieeConst Implementation |------------//


Tissue::NotifieeConst::~NotifieeConst() {
   if(notifier_) {
      notifier_->deleteNotifiee(this);
   }
   if(notifier_&&isNonReferencing()) notifier_->newRef();
}

void
Tissue::NotifieeConst::notifierIs(const Tissue::PtrConst& _notifier) {
   Tissue::Ptr notifierSave(const_cast<Tissue *>(notifier_.ptr()));
   if(_notifier==notifier_) return;
   notifier_ = _notifier;
   if(notifierSave) {
      notifierSave->deleteNotifiee(this);
   }
   if(_notifier) {
      _notifier->newNotifiee(this);
   }
   if(isNonReferencing_) {
      if(notifierSave) notifierSave->newRef();
      if(notifier_) notifier_->deleteRef();
   }
}

void
Tissue::NotifieeConst::isNonReferencingIs(bool _isNonReferencing){
   if(_isNonReferencing==isNonReferencing_) return;
   isNonReferencing_ = _isNonReferencing;
   if(notifier_) {
      if(_isNonReferencing) notifier_->deleteRef();
      else notifier_->newRef();
   }
   }

//----------| Notifiee Implementation |------------//

Fwk::String
Tissue::attributeString( Fwk::RootNotifiee::AttributeId a ) const {
   Fwk::String str = "unknown";
   switch(a) {
      case NotifieeConst::version__: str = "version"; break;
      case NotifieeConst::notificationException__: str = "notificationException"; break;
      case NotifieeConst::cell__: str = "cell"; break;
      default: str = Fwk::RootNotifiee::attributeString(a);
   }
   return str;
}


Tissue::~Tissue() {
  CellIterator i = cellIter();
   while(i.ptr())
   {
	 cellDel(i->fwkKey());
	 i = cellIter();
   }
}

Cell::Ptr
Tissue::cellDel(Fwk::String _name) {
   Cell::Ptr m = cell_.deleteMember(_name);
   if(!m) return 0;
   m->tissueIs(0);
   retrycellDel:
   U32 ver = notifiee_.version();
   if(notifiees()) for(NotifieeIterator n=notifieeIter();n.ptr();++n) try {
      n->onCellDel(_name);
      n->onCellDel(m);
      if( ver != notifiee_.version() ) goto retrycellDel;
   } catch(...) { n->onNotificationException(NotifieeConst::cell__); }
   return m;
}

Cell::Ptr
Tissue::cellIs(Cell::Ptr cell) {
  Fwk::String name = cell->location().name();
  Cell::Ptr m = cell_[name];
   if(m) {
      throw Fwk::NameInUseException(name);
   } else {
     m = cell;
     cell_.newMember(m);
   }
   retrycell:
   U32 ver = notifiee_.version();
   if(notifiees()) for(NotifieeIterator n=notifieeIter();n.ptr();++n) try {
      n->onCellNew(cell);
      if( ver != notifiee_.version() ) goto retrycell;
   } catch(...) { n->onNotificationException(NotifieeConst::cell__); }
   return cell;
}

Tissue::Tissue(Fwk::String _name): Fwk::NamedInterface(_name) {

}

//----------| TCell Implementation |------------//

//----------| NotifieeConst Implementation |------------//

//----------| Notifiee Implementation |------------//

Fwk::String
TCell::attributeString( Fwk::RootNotifiee::AttributeId a ) const {
   Fwk::String str = "unknown";
   switch(a) {
      case NotifieeConst::version__: str = "version"; break;
      case NotifieeConst::notificationException__: str = "notificationException"; break;
      case NotifieeConst::membrane__: str = "membrane"; break;
      case NotifieeConst::tissue__: str = "tissue"; break;
      default: str = Fwk::RootNotifiee::attributeString(a); break;
   }
   return str;
}

TCell::TCell(Coordinates _loc, Tissue * _tissue, Cell::CellType _type): Cell(_loc, _tissue, _type) {
}

//----------| CytotoxicCell Implementation |------------//

//----------| NotifieeConst Implementation |------------//

//----------| Notifiee Implementation |------------//

Fwk::String
CytotoxicCell::attributeString( Fwk::RootNotifiee::AttributeId a ) const {
   Fwk::String str = "unknown";
   switch(a) {
      case NotifieeConst::version__: str = "version"; break;
      case NotifieeConst::notificationException__: str = "notificationException"; break;
      case NotifieeConst::membrane__: str = "membrane"; break;
//      case NotifieeConst::neighborMembrane__: str = "neighborMembrane"; break;
      case NotifieeConst::tissue__: str = "tissue"; break;
      default: str = Fwk::RootNotifiee::attributeString(a);
   }
   return str;
}


CytotoxicCell::CytotoxicCell(Coordinates _loc, Tissue * _tissue):
  TCell(_loc, _tissue, Cell::cytotoxicCell_) {
}

//----------| HelperCell Implementation |------------//

//----------| NotifieeConst Implementation |------------//

//----------| Notifiee Implementation |------------//

Fwk::String
HelperCell::attributeString( Fwk::RootNotifiee::AttributeId a ) const {
   Fwk::String str = "unknown";
   switch(a) {
      case NotifieeConst::version__: str = "version"; break;
      case NotifieeConst::notificationException__: str = "notificationException"; break;
      case NotifieeConst::membrane__: str = "membrane"; break;
      case NotifieeConst::tissue__: str = "tissue"; break;
      default: str = Fwk::RootNotifiee::attributeString(a); break;
   }
   return str;
}
HelperCell::HelperCell(Coordinates _loc, Tissue * _tissue) :
  TCell(_loc, _tissue, Cell::helperCell_){}

Fwk::String valueToStrep(CellMembrane::Side s) { return stringValue(s); }
Fwk::String valueToStrep(Fwk::String s) { return s; }
