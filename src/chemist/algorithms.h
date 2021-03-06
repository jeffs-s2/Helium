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
#ifndef HELIUM_CHEMIST_ALGORITHMS_H
#define HELIUM_CHEMIST_ALGORITHMS_H

#include <Helium/distancematrix.h>
#include <Helium/algorithms/bfs.h>
#include <Helium/algorithms/canonical.h>
#include <Helium/algorithms/components.h>
#include <Helium/algorithms/dfs.h>
#include <Helium/algorithms/dijkstra.h>
#include <Helium/algorithms/enumeratepaths.h>
#include <Helium/algorithms/enumeratesubgraphs.h>
#include <Helium/algorithms/extendedconnectivities.h>
#include <Helium/algorithms/floydwarshall.h>
#include <Helium/algorithms/gtd.h>
#include <Helium/algorithms/invariants.h>
#include <Helium/algorithms/isomorphism.h>
#include <Helium/algorithms/kekulize.h>
#include <Helium/algorithms/aromatize.h>

namespace Helium {

  namespace Chemist {

    // distancematrix.h
    using Helium::DistanceMatrix;

    // bfs.h
    using Helium::BFSVisitor;
    using Helium::BFSAtomOrderVisitor;
    using Helium::BFSBondOrderVisitor;
    using Helium::BFSClosureRecorderVisitor;
    using Helium::BFSDebugVisitor;

    // dfs.h
    using Helium::DFSVisitor;
    using Helium::DFSAtomOrderVisitor;
    using Helium::DFSBondOrderVisitor;
    using Helium::DFSClosureRecorderVisitor;
    using Helium::DFSDebugVisitor;

    // dijkstra.h
    using Helium::Dijkstra;

    // enumeratesubgraphs.h
    using Helium::Subgraph;

    // invariants.h
    using Helium::DefaultAtomInvariant;
    using Helium::DefaultBondInvariant;

    // isomorphism.h
    using Helium::IsomorphismMapping;
    using Helium::IsomorphismMappings;
    using Helium::NoMapping;
    using Helium::CountMapping;
    using Helium::SingleMapping;
    using Helium::MappingList;
    using Helium::DefaultAtomMatcher;
    using Helium::DefaultBondMatcher;

  }

}

#endif
