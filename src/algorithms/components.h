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
#ifndef HELIUM_COMPONENTS_H
#define HELIUM_COMPONENTS_H

#include <Helium/molecule.h>
#include <Helium/util.h>

#include <vector>

namespace Helium {

  /**
   * @file algorithms/components.h
   * @brief Connected components.
   */

  namespace impl {

    /**
     * Find all bond belonging to the same component using a recursive DFS search.
     */
    template<typename MoleculeType>
    void connected_bond_components(const MoleculeType &mol, typename molecule_traits<MoleculeType>::atom_type atom,
        unsigned int number, std::vector<unsigned int> &components)
    {
      // iterator over atom's bonds
      for (auto &bond : get_bonds(mol, atom)) {
        // skip already visited bonds
        if (components[get_index(mol, bond)] != molecule_traits<MoleculeType>::null_index())
          continue;
        // assign component to bond
        components[get_index(mol, bond)] = number;
        // recursive call
        connected_bond_components(mol, get_other(mol, bond, atom), number, components);
      }
    }

  }

  /**
   * Get a std::vector containing the component number for each bond. This
   * vector is indexed by bond index starting from 0. The components are
   * sequentially numbered starting from 0.
   *
   * @param mol The molecule.
   *
   * @return A std::vector containing the component number for each bond.
   */
  template<typename MoleculeType>
  std::vector<unsigned int> connected_bond_components(const MoleculeType &mol)
  {
    unsigned int number = 0;
    std::vector<unsigned int> components(num_bonds(mol), molecule_traits<MoleculeType>::null_index());

    // start searching from each (non-visited) bond
    for (auto &bond : get_bonds(mol)) {
      if (components[get_index(mol, bond)] != molecule_traits<MoleculeType>::null_index())
        continue;
      // initiate recursion for each component
      impl::connected_bond_components(mol, get_source(mol, bond), number++, components);
    }

    return components;
  }

  /**
   * Get a std::vector containing the component number for each atom. This
   * vector is indexed by atom index starting from 0. The components are
   * sequentially numbered starting from 0.
   *
   * @param mol The molecule.
   *
   * @return A std::vector containing the component number for each atom.
   */
  template<typename MoleculeType>
  std::vector<unsigned int> connected_atom_components(const MoleculeType &mol)
  {
    std::vector<unsigned int> atom_components(num_atoms(mol), molecule_traits<MoleculeType>::null_index());

    // convert bond components to atom components
    std::vector<unsigned int> bond_components(connected_bond_components(mol));
    for (auto &bond : get_bonds(mol)) {
      unsigned int number = bond_components[get_index(mol, bond)];
      atom_components[get_index(mol, get_source(mol, bond))] = number;
      atom_components[get_index(mol, get_target(mol, bond))] = number;
    }

    // handle isolated atoms
    unsigned int number = unique_elements(atom_components) - 1;
    for (auto &atom : get_atoms(mol))
      if (atom_components[get_index(mol, atom)] == molecule_traits<MoleculeType>::null_index())
        atom_components[get_index(mol, atom)] = number++;

    return atom_components;
  }

  /**
   * Get the number of connected components in a molecule.
   *
   * @param mol The molecule.
   *
   * @return The number of connected components.
   */
  template<typename MoleculeType>
  Size num_connected_components(const MoleculeType &mol)
  {
    return unique_elements(connected_atom_components(mol));
  }

}

#endif
