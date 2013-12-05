/*
 * Copyright (c) 2013, Tim Vandermeersch
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef HELIUM_SMIRKS_H
#define HELIUM_SMIRKS_H

#include <Helium/smarts.h>

#include <iostream>

#define DEBUG_SMIRKS 0

namespace Helium {

  /**
   * @brief Class representing possible SMIRKS errors.
   */
  class SmirksError
  {
    public:
      /**
       * @brief The error type.
       */
      enum Type {
        /**
         * @brief No error.
         */
        None,
        /**
         * @brief The SMIRKS does not contain '>>'.
         */
        NoReaction,
        /**
         * @brief Parse error in reactant SMARTS.
         */
        ReactantSmarts,
        /**
         * @brief Parse error in product SMARTS.
         */
        ProductSmarts,
        /**
         * @brief The atom classes are not pair-wise.
         */
        AtomClassPairWise,
        /**
         * @brief The product SMARTS contains an OR expression.
         */
        ProductContainsOr,
        /**
         * @brief The product SMARTS contains a NOT expression.
         */
        ProductContainsNot,
        /**
         * @brief The product contains a complex bond expression.
         */
        InvalidProductBond,
        /**
         * @brief The product contains a conflict (e.g. [CN]).
         */
        ProductConflict
      };

      /**
       * @brief Default constructor.
       *
       * This constructor initializes the error to None.
       */
      SmirksError() : m_type(None)
      {
      }

      /**
       * @brief Constructor.
       *
       * @param type The error type.
       * @param what The error message.
       */
      SmirksError(Type type, const std::string &what) : m_type(type), m_what(what)
      {
      }

      /**
       * @brief Get the error type.
       *
       * @return The error type.
       */
      Type type() const
      {
        return m_type;
      }

      /**
       * @brief Get the error message.
       *
       * @return The error message.
       */
      const std::string& what() const
      {
        return m_what;
      }

    private:
      Type m_type; //!< The error type.
      std::string m_what; //!< The error message.
  };

  namespace impl {

    template<typename ExprType>
    int smarts_get_element(ExprType *expr)
    {
      int value;
      switch (expr->type) {
        case Smiley::OP_AndHi:
        case Smiley::OP_AndLo:
        case Smiley::OP_And:
          value = smarts_get_element(expr->left);
          if (value != -1)
            return value;
          value = smarts_get_element(expr->right);
          if (value != -1)
            return value;
          break;
        case Smiley::AE_AtomicNumber:
        case Smiley::AE_AromaticElement:
        case Smiley::AE_AliphaticElement:
          return expr->value;
        default:
          break;
      }

      return -1;
    }

  }

  /**
   * @brief Class for applying SMIRKS transformations.
   */
  class Smirks
  {
      /**
       * @brief Class representing a change to a bond.
       */
      struct BondChange
      {
        /**
         * @brief The type of change.
         */
        enum Type {
          /**
           * @brief Bond is changed.
           */
          Changed,
          /**
           * @brief Bond is added.
           */
          Added,
          /**
           * @brief Bond is removed.
           */
          Removed
        };

        /**
         * @brief Constructor.
         *
         * @param type_ The error type.
         * @param source_ The reactant source index.
         * @param target_ The reactant target index.
         * @param expr_ The product SMARTS bond expression.
         */
        BondChange(int type_, int source_, int target_, impl::SmartsBondExpr *expr_)
          : type(type_), source(source_), target(target_), expr(expr_)
        {
        }

        int type; //!< The type of change.
        int source; //!< Reactant source index.
        int target; //!< Reactant target index.
        impl::SmartsBondExpr *expr; //!< Product SMARTS bond expression.
      };

    public:
      /**
       * @brief Constructor.
       */
      Smirks() : m_fixMass(true), m_fixHydrogens(true)
      {
      }

      /**
       * @brief Enable/diable fixing of mass.
       *
       * @param value True for enable, false for diable
       */
      void setFixMass(bool value)
      {
        m_fixMass = value;
      }

      /**
       * @brief Enable/diable fixing of hydrogens.
       *
       * @param value True for enable, false for diable
       */
      void setFixHydrogens(bool value)
      {
        m_fixHydrogens = value;
      }

      /**
       * @brief Initialize the SMIRKS.
       *
       * @param smirks The SMIRKS string.
       *
       * @return True if successful.
       */
      bool init(const std::string &smirks);

      /**
       * @brief Initialize the SMIRKS.
       *
       * @param reactant The reactant SMARTS string.
       * @param product The product SMARTS string.
       *
       * @return True if successful.
       */
      bool init(const std::string &reactant, const std::string &product);

      /**
       * @brief Get the error from the last call to init().
       *
       * @return The SMIRKS erorr.
       */
      const SmirksError& error() const
      {
        return m_error;
      }

      /**
       * @brief Check if the reactant SMARTS has cycle atom/bond primitives elements.
       *
       * Examples are [R3], [r5], [x2], *@*.
       *
       * @return True if the reactant SMARTS has cylce atom/bond primitives.
       */
      bool requiresCycles()
      {
        return m_reactant.requiresCycles();
      }

      /**
       * @brief Check if the reactant SMARTS has explicit hydrogens.
       *
       * @return True if the reactant SMARTS has explicit hydrogens.
       */
      bool requiresExplicitHydrogens()
      {
        return m_reactant.requiresExplicitHydrogens();
      }

      /**
       * @brief Apply the SMIRKS transformation to a molecule.
       *
       * @param mol The molecule.
       * @param rings The molecule's rings (needed for cyclic queries).
       *
       * @return True if changes were made to the molecule.
       */
      template<typename EditableMoleculeType>
      bool apply(EditableMoleculeType &mol, const RingSet<EditableMoleculeType> &rings)
      {
        if (DEBUG_SMIRKS)
          std::cout << "Smirks::apply()" << std::endl;

        MappingList mapping;
        if (!m_reactant.search(mol, mapping, rings, true))
          return false;

        //
        // apply atom changes (atom's with atom class)
        //
        for (std::size_t i = 0; i < mapping.maps.size(); ++i) {
          const IsomorphismMapping &map = mapping.maps[i];
          for (std::size_t j = 0; j < map.size(); ++j) {
            int atomClass = m_reactant.atomClass(j);
            if (atomClass == -1)
              continue;

            impl::SmartsAtomExpr *productExpr = m_productExpr[atomClass];

            apply(mol, get_atom(mol, map[j]), productExpr);
          }
        }

        std::vector<Index> remove;

        //
        // apply bond changes (bonds between atoms with atom class)
        //
        for (std::size_t i = 0; i < mapping.maps.size(); ++i) {
          const IsomorphismMapping &map = mapping.maps[i];

          for (std::size_t j = 0; j < m_bondChanges.size(); ++j) {
            const BondChange &change = m_bondChanges[j];

            switch (change.type) {
              case BondChange::Changed:
                {
                  typename molecule_traits<EditableMoleculeType>::atom_type source = get_atom(mol, map[change.source]);
                  typename molecule_traits<EditableMoleculeType>::atom_type target = get_atom(mol, map[change.target]);
                  typename molecule_traits<EditableMoleculeType>::bond_type bond = get_bond(mol, source, target);
                  apply(mol, bond, change.expr);

                  if (DEBUG_SMIRKS)
                    std::cout << "changed bond " << get_index(mol, bond) << ": " << get_index(mol, source) << "-" << get_index(mol, target) << std::endl;
                }
                break;
              case BondChange::Removed:
                {
                  typename molecule_traits<EditableMoleculeType>::atom_type source = get_atom(mol, map[change.source]);
                  typename molecule_traits<EditableMoleculeType>::atom_type target = get_atom(mol, map[change.target]);
                  typename molecule_traits<EditableMoleculeType>::bond_type bond = get_bond(mol, source, target);
                  remove.push_back(get_index(mol, bond));

                  if (DEBUG_SMIRKS)
                    std::cout << "removed bond " << get_index(mol, bond) << ": " << get_index(mol, source) << "-" << get_index(mol, target) << std::endl;
                }
                break;
              case BondChange::Added:
                {
                  typename molecule_traits<EditableMoleculeType>::atom_type source = get_atom(mol, map[change.source]);
                  typename molecule_traits<EditableMoleculeType>::atom_type target = get_atom(mol, map[change.target]);
                  typename molecule_traits<EditableMoleculeType>::bond_type bond = add_bond(mol, source, target);
                  apply(mol, bond, change.expr);

                  if (DEBUG_SMIRKS)
                    std::cout << "added bond " << get_index(mol, bond) << ": " << get_index(mol, source) << "-" << get_index(mol, target) << std::endl;
                }
                break;
            }
          }
        }

        // remove the planned bonds
        std::sort(remove.begin(), remove.end(), std::greater<Index>());
        for (std::size_t i = 0; i < remove.size(); ++i)
          remove_bond(mol, get_bond(mol, remove[i]));

        //
        // Remove reactant atoms that do not have atom classes
        //
        remove.clear();
        for (std::size_t i = 0; i < mapping.maps.size(); ++i) {
          const IsomorphismMapping &map = mapping.maps[i];
          for (std::size_t j = 0; j < map.size(); ++j) {
            int atomClass = m_reactant.atomClass(j);
            if (atomClass == -1)
              remove.push_back(mapping.maps[i][j]);
          }
        }

        //
        // Add product atoms that do not have atom classes
        //
        std::map<Index, std::vector<Index> > product2mol; // unmapped product atom index -> added atom index in mol
        for (std::size_t i = 0; i < mapping.maps.size(); ++i)
          for (std::size_t j = 0; j < num_atoms(m_product.query()); ++j) {
            int atomClass = m_product.atomClass(j);
            if (atomClass != -1)
              continue;

            if (product2mol.find(j) == product2mol.end())
              product2mol[j] = std::vector<Index>();

            typename molecule_traits<EditableMoleculeType>::atom_type atom = add_atom(mol);
            apply(mol, atom, m_product.trees().atom(j));
            product2mol[j].push_back(get_index(mol, atom));
          }

        // make a map of reactant & reactant atom index to atom class
        std::map<Index, Index> reactantAtomClass2index;
        for (std::size_t i = 0; i < num_atoms(m_reactant.query()); ++i) {
          int atomClass = m_reactant.atomClass(i);
          if (atomClass == -1)
            continue;
          reactantAtomClass2index[atomClass] = i;
        }

        //
        // Add product bonds that do not have atom clases
        //
        for (std::size_t i = 0; i < mapping.maps.size(); ++i)
          for (std::size_t j = 0; j < num_bonds(m_product.query()); ++j) {
            molecule_traits<HeMol>::bond_type bond = get_bond(m_product.query(), j);
            molecule_traits<HeMol>::atom_type source = get_source(m_product.query(), bond);
            molecule_traits<HeMol>::atom_type target = get_target(m_product.query(), bond);
            int sourceIndex = get_index(m_product.query(), source);
            int targetIndex = get_index(m_product.query(), target);
            int sourceAtomClass = m_product.atomClass(sourceIndex);
            int targetAtomClass = m_product.atomClass(targetIndex);


            // both atoms have atom class: ignore bond
            if (sourceAtomClass != -1 && targetAtomClass != -1)
              continue;


            // both atoms do not have atom class: use product2mol
            if (sourceAtomClass == -1 && targetAtomClass == -1) {
              typename molecule_traits<EditableMoleculeType>::atom_type molSource = get_atom(mol, product2mol[sourceIndex][i]);
              typename molecule_traits<EditableMoleculeType>::atom_type molTarget = get_atom(mol, product2mol[targetIndex][i]);
              typename molecule_traits<EditableMoleculeType>::bond_type molBond = add_bond(mol, molSource, molTarget);
              apply(mol, molBond, m_product.trees().bond(j));
              continue;
            }


            // one atom has atom class: use product2mol + reactantAtomClass2index -> mapping
            Index index = sourceAtomClass == -1 ? sourceIndex : targetIndex;
            int atomClass = sourceAtomClass == -1 ? targetAtomClass : sourceAtomClass;

            typename molecule_traits<EditableMoleculeType>::atom_type molSource = get_atom(mol, product2mol[index][i]);
            typename molecule_traits<EditableMoleculeType>::atom_type molTarget = get_atom(mol, mapping.maps[i][reactantAtomClass2index[atomClass]]);
            typename molecule_traits<EditableMoleculeType>::bond_type molBond = add_bond(mol, molSource, molTarget);
            apply(mol, molBond, m_product.trees().bond(j));
          }

        // remove the planned atoms
        std::sort(remove.begin(), remove.end(), std::greater<Index>());
        for (std::size_t i = 0; i < remove.size(); ++i)
          remove_atom(mol, get_atom(mol, remove[i]));

        if (m_fixHydrogens)
          reset_implicit_hydrogens(mol);

        return true;
      }

    private:
      template<typename EditableMoleculeType, typename AtomType>
      void apply(EditableMoleculeType &mol, AtomType atom, impl::SmartsAtomExpr *expr)
      {
        int oldElement = get_element(mol, atom);
        applyRecursive(mol, atom, expr);
        if (m_fixMass) {
          int newElement = impl::smarts_get_element(expr);
          if (newElement != -1 && newElement != oldElement)
            set_mass(mol, atom, Element::averageMass(newElement));
        }
      }

      /**
       * @brief Apply changes to an atom.
       *
       * @param mol The molecule.
       * @param atom The atom to change.
       * @param expr The product SMARTS atom expression.
       */
      template<typename EditableMoleculeType, typename AtomType>
      void applyRecursive(EditableMoleculeType &mol, AtomType atom, impl::SmartsAtomExpr *expr)
      {
        switch (expr->type) {
          case Smiley::OP_AndHi:
          case Smiley::OP_AndLo:
          case Smiley::OP_And:
          case Smiley::OP_Or:
            applyRecursive(mol, atom, expr->left);
            applyRecursive(mol, atom, expr->right);
            break;
          case Smiley::AE_Isotope:
            set_mass(mol, atom, expr->value);
            break;
          case Smiley::AE_AtomicNumber:
            set_element(mol, atom, expr->value);
            break;
          case Smiley::AE_AromaticElement:
            set_element(mol, atom, expr->value);
            set_aromatic(mol, atom, true);
            break;
          case Smiley::AE_AliphaticElement:
            set_element(mol, atom, expr->value);
            set_aromatic(mol, atom, false);
            break;
          case Smiley::AE_TotalH:
            {
              int explicitH = 0;
              FOREACH_NBR (nbr, atom, mol, EditableMoleculeType)
                if (is_hydrogen(mol, *nbr))
                  ++explicitH;
              set_hydrogens(mol, atom, expr->value - explicitH);
            }
            break;
          case Smiley::AE_ImplicitH:
            set_hydrogens(mol, atom, expr->value);
            break;
          case Smiley::AE_Charge:
            set_charge(mol, atom, expr->value);
            break;
          case Smiley::AE_Chirality:
          case Smiley::OP_Not:
          default:
            break;
        }
      }

      /**
       * @brief Apply changes to a bond.
       *
       * @param mol The molecule.
       * @param bond The bond to change.
       * @param expr The product SMARTS bond expression.
       */
      template<typename EditableMoleculeType, typename BondType>
      void apply(EditableMoleculeType &mol, BondType bond, impl::SmartsBondExpr *expr)
      {
        switch (expr->type) {
          case Smiley::BE_Single:
            set_order(mol, bond, 1);
            set_aromatic(mol, bond, false);
            break;
          case Smiley::BE_Double:
            set_order(mol, bond, 2);
            set_aromatic(mol, bond, false);
            break;
          case Smiley::BE_Triple:
            set_order(mol, bond, 3);
            set_aromatic(mol, bond, false);
            break;
          case Smiley::BE_Quadriple:
            set_order(mol, bond, 4);
            set_aromatic(mol, bond, false);
            break;
          case Smiley::BE_Aromatic:
            set_order(mol, bond, 5);
            set_aromatic(mol, bond, true);
            break;
          case Smiley::BE_Up:
          case Smiley::BE_Down:
            break;
        }
      }

      /**
       * @brief Extract atom class to SMARTS expression mapping from SMARTS object.
       *
       * @param smarts The SMARTS object.
       *
       * @return The atom class to SMARTS expression mapping.
       */
      std::map<int, impl::SmartsAtomExpr*> atomClassToExpr(const Smarts &smarts) const;

      /**
       * @brief Find a bond in a SMARTS between the specified atom classes.
       *
       * @param smarts The SMARTS object.
       * @param sourceAtomClass The source atom class.
       * @param targetAtomClass The target atom class.
       *
       * @return The found bond or molecule_traits<HeMol>::null_bond().
       */
      molecule_traits<HeMol>::bond_type getBond(const Smarts &smarts, int sourceAtomClass, int targetAtomClass);

      Smarts m_reactant; //!< The reactant SMARTS.
      Smarts m_product; //!< The product SMARTS.
      std::map<int, impl::SmartsAtomExpr*> m_reactantExpr; //!< Reactant atom class to atom expr.
      std::map<int, impl::SmartsAtomExpr*> m_productExpr; //!< Product atom class to atom expr.
      std::vector<BondChange> m_bondChanges; //!< The bond changes.
      SmirksError m_error; //!< Error from last call to init().
      bool m_fixMass;
      bool m_fixHydrogens;
  };

}

#endif
