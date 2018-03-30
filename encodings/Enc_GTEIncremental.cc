#include "Enc_GTEIncremental.h"
#include <algorithm>
#include <numeric>
using namespace openwbo;

struct less_than_wlitt {
  inline bool operator()(const wlitt &wl1, const wlitt &wl2) {
    return (wl1.weight < wl2.weight);
  }
};

Lit GTEIncremental::getNewLit(Solver *S) {
  Lit p = mkLit(S->nVars(), false);
  newSATVariable(S);
  nb_variables++;
  return p;
}

Lit GTEIncremental::get_var(Solver *S, wlit_mapt &oliterals, uint64_t weight) {
  wlit_mapt::iterator it = oliterals.find(weight);
  if (it == oliterals.end()) {
    Lit v = getNewLit(S);
    oliterals[weight] = v;
  }
  return oliterals[weight];
}

bool GTEIncremental::encodeLeq(uint64_t k, Solver *S, const weightedlitst &iliterals,
                    wlit_mapt &oliterals) {

  if (iliterals.size() == 0 || k == 0)
    return false;

  if (iliterals.size() == 1) {

    oliterals.insert(
        wlit_pairt(iliterals.front().weight, iliterals.front().lit));
    return true;
  }

  unsigned int size = iliterals.size();

  // formulat lformula,rformula;
  weightedlitst linputs, rinputs;
  wlit_mapt loutputs, routputs;

  unsigned int lsize = size >> 1;
  // unsigned int rsize=size-lsize;
  weightedlitst::const_iterator myit = iliterals.begin();
  weightedlitst::const_iterator myit1 = myit + lsize;
  weightedlitst::const_iterator myit2 = iliterals.end();

  linputs.insert(linputs.begin(), myit, myit1);
  rinputs.insert(rinputs.begin(), myit1, myit2);

  /*wlitt init_wlit;
  init_wlit.lit = lit_Undef;
  init_wlit.weight=0;*/
  wlit_sumt wlit_sum;
  uint64_t lk = std::accumulate(linputs.begin(), linputs.end(), 0, wlit_sum);
  uint64_t rk = std::accumulate(rinputs.begin(), rinputs.end(), 0, wlit_sum);

  lk = k >= lk ? lk : k;
  rk = k >= rk ? rk : k;

  bool result = encodeLeq(lk, S, linputs, loutputs);
  if (!result)
    return result;
  result = result && encodeLeq(rk, S, rinputs, routputs);
  if (!result)
    return result;

  {
    assert(!loutputs.empty());

    for (wlit_mapt::iterator mit = loutputs.begin(); mit != loutputs.end();
         mit++) {

      if (mit->first > k) {
        addBinaryClause(S, ~mit->second, get_var(S, oliterals, k));
        nb_clauses++;
      } else {
        addBinaryClause(S, ~mit->second, get_var(S, oliterals, mit->first));
        nb_clauses++;
        // clause.push_back(get_var(auxvars,oliterals,l.first));
      }

      // formula.push_back(std::move(clause));
    }
  }

  {
    assert(!routputs.empty());
    for (wlit_mapt::iterator mit = routputs.begin(); mit != routputs.end();
         mit++) {

      if (mit->first > k) {
        addBinaryClause(S, ~mit->second, get_var(S, oliterals, k));
        nb_clauses++;
        // clause.push_back(get_var(auxvars,oliterals,k));
      } else {
        addBinaryClause(S, ~mit->second, get_var(S, oliterals, mit->first));
        nb_clauses++;
        // clause.push_back(get_var(auxvars,oliterals,r.first));
      }

      // formula.push_back(std::move(clause));
    }
  }

  // if(!lformula.empty() && !rformula.empty())
  {
    for (wlit_mapt::iterator lit = loutputs.begin(); lit != loutputs.end();
         lit++) {
      for (wlit_mapt::iterator rit = routputs.begin(); rit != routputs.end();
           rit++) {
        /*clauset clause;
        clause.push_back(-l.second);
        clause.push_back(-r.second);*/
        uint64_t tw = lit->first + rit->first;
        if (tw > k) {
          addTernaryClause(S, ~lit->second, ~rit->second,
                           get_var(S, oliterals, k));
          nb_clauses++;
          // clause.push_back(get_var(auxvars,oliterals,k));
        } else {
          addTernaryClause(S, ~lit->second, ~rit->second,
                           get_var(S, oliterals, tw));
          nb_clauses++;
          // clause.push_back(get_var(auxvars,oliterals,tw));
        }

        // formula.push_back(std::move(clause));
      }
    }
  }
  
  return true;
}

bool GTEIncremental::encodeLeqIncremental(uint64_t k, Solver *S, const weightedlitst &iliterals,
                    wlit_mapt &oliterals) {
                    	
  if (iliterals.size() == 0 || k == 0)
    return false;

  if (iliterals.size() == 1) {

    oliterals.insert(
        wlit_pairt(iliterals.front().weight, iliterals.front().lit));
    return true;
  }

  unsigned int size = iliterals.size();

  // formulat lformula,rformula;
  weightedlitst linputs, rinputs;
  wlit_mapt loutputs, routputs;

  unsigned int lsize = size >> 1;
  // unsigned int rsize=size-lsize;
  weightedlitst::const_iterator myit = iliterals.begin();
  weightedlitst::const_iterator myit1 = myit + lsize;
  weightedlitst::const_iterator myit2 = iliterals.end();

  linputs.insert(linputs.begin(), myit, myit1);
  rinputs.insert(rinputs.begin(), myit1, myit2);

  /*wlitt init_wlit;
  init_wlit.lit = lit_Undef;
  init_wlit.weight=0;*/
  wlit_sumt wlit_sum;
  uint64_t lk = std::accumulate(linputs.begin(), linputs.end(), 0, wlit_sum);
  uint64_t rk = std::accumulate(rinputs.begin(), rinputs.end(), 0, wlit_sum);

  lk = k >= lk ? lk : k;
  rk = k >= rk ? rk : k;

  bool result = encodeLeqIncremental(lk, S, linputs, loutputs);
  if (!result)
    return result;
  result = result && encodeLeqIncremental(rk, S, rinputs, routputs);
  if (!result)
    return result;
    
  bool added_first_above_k = false;
  uint64_t least_weight_above_k = 0;
  
  assert(!loutputs.empty());
  assert(!routputs.empty());
  
  uint64_t left_largest_weight = loutputs.rbegin()->first;
  uint64_t right_largest_weight = routputs.rbegin()->first;
  

  least_weight_above_k = loutputs.rbegin()->first + routputs.rbegin()->first;
  if(least_weight_above_k > k) {
    added_first_above_k = true;
    for (wlit_mapt::iterator lit = loutputs.begin(); lit != loutputs.end();
       lit++) {
      for (wlit_mapt::iterator rit = routputs.begin(); rit != routputs.end();
         rit++) {
        uint64_t tw = lit->first + rit->first;
        printf("TW : %d\n",tw);
        if(tw > k && tw < least_weight_above_k) {
          printf("Setting %d, earlier was %d, k is %d\n\n",tw,least_weight_above_k,k);
          least_weight_above_k = tw;
        }
      }
    }
  }
  
  if(added_first_above_k) {
    if(left_largest_weight > k) {
      if(right_largest_weight > k && 
        right_largest_weight < left_largest_weight &&
        right_largest_weight < least_weight_above_k) {
        least_weight_above_k = right_largest_weight;
      } else if(left_largest_weight < least_weight_above_k) {
        least_weight_above_k = left_largest_weight;
      }
    } else if(right_largest_weight > k && 
              right_largest_weight < least_weight_above_k) {
      least_weight_above_k = right_largest_weight;
    }
  }
  

  {
    
    for (wlit_mapt::iterator mit = loutputs.begin(); mit != loutputs.end();
         mit++) {

      if (mit->first > k) {
        assert(added_first_above_k);
        implication(mit->first,least_weight_above_k);
        addBinaryClause(S, ~mit->second, 
          get_var(S, oliterals, least_weight_above_k));
        nb_clauses++;
      } else {
        implication(mit->first,mit->first);
        addBinaryClause(S, ~mit->second, get_var(S, oliterals, mit->first));
        nb_clauses++;
        // clause.push_back(get_var(auxvars,oliterals,l.first));
      }/* else {
        // just creating the literal
        // TODO - NO! This is wrong! You don't create a new variable each time!
        get_var(S, oliterals, mit->first);
      }*/

      // formula.push_back(std::move(clause));
    }
  }

  {
    
    for (wlit_mapt::iterator mit = routputs.begin(); mit != routputs.end();
         mit++) {

      if (mit->first > k) {
        assert(added_first_above_k);
        implication(mit->first,least_weight_above_k);
        addBinaryClause(S, ~mit->second,
          get_var(S, oliterals, least_weight_above_k));
        nb_clauses++;
        // clause.push_back(get_var(auxvars,oliterals,k));
      } else {
        implication(mit->first,mit->first);
        addBinaryClause(S, ~mit->second, get_var(S, oliterals, mit->first));
        nb_clauses++;
        // clause.push_back(get_var(auxvars,oliterals,r.first));
      }/*else {
        // just creating the literal
        get_var(S, oliterals, mit->first);
      } */

      // formula.push_back(std::move(clause));
    }
  }

  // if(!lformula.empty() && !rformula.empty())
  {
    // sending pairwise sums to parent
    for (wlit_mapt::iterator lit = loutputs.begin(); lit != loutputs.end();
         lit++) {
      for (wlit_mapt::iterator rit = routputs.begin(); rit != routputs.end();
           rit++) {
        /*clauset clause;
        clause.push_back(-l.second);
        clause.push_back(-r.second);*/
        uint64_t tw = lit->first + rit->first;
        if (tw > k) {
          assert(added_first_above_k);
          implication(lit->first,rit->first,least_weight_above_k);
          addTernaryClause(S, ~lit->second, ~rit->second,
                           get_var(S, oliterals, least_weight_above_k)); // TODO - check
          nb_clauses++;
          // clause.push_back(get_var(auxvars,oliterals,k));
        } else {
          implication(lit->first,rit->first,tw);
          addTernaryClause(S, ~lit->second, ~rit->second,
                           get_var(S, oliterals, tw));
          nb_clauses++;
          // clause.push_back(get_var(auxvars,oliterals,tw));
        }/* else {
          // just creating the literal
          get_var(S, oliterals, tw);
        } */

        // formula.push_back(std::move(clause));
      }
    }
  }
  
  // TODO - check if needed
/*  less_than_map sort_map;
  std::sort(oliterals.begin(), oliterals.end(), sort_map); */
  
  for(wlit_mapt::reverse_iterator oit = ++(oliterals.rbegin());
      oit != oliterals.rend(); oit++) {
  /*  if(oit->first <= old_k) {
      // these implications have been added before
      break;
    } */
    wlit_mapt::reverse_iterator implied_lit = oit;
    --implied_lit;
    implication(oit->first,implied_lit->first);
    addBinaryClause(S, ~oit->second, implied_lit->second);
    nb_clauses++;
  }
  
  printf("At this level ############################################\n");
  printf("LEFT\n");
  for (wlit_mapt::iterator mit = loutputs.begin(); mit != loutputs.end();
         mit++) {
    printf("%d ",mit->first);
  }
  printf("\nRIGHT\n");
  for (wlit_mapt::iterator mit = routputs.begin(); mit != routputs.end();
         mit++) {
    printf("%d ",mit->first);
  }
  printf("\nOUTPUT\n");
  for (wlit_mapt::iterator mit = oliterals.begin(); mit != oliterals.end();
         mit++) {
    printf("%d ",mit->first);
  }
  printf("\n\n#######################################################\n\n\n");
  adder(loutputs, routputs, oliterals, k);

  return true;
}

void GTEIncremental::build(Solver *S, vec<Lit> &lits, vec<uint64_t> &coeffs,
				uint64_t rhs) {
	vec<Lit> simp_lits;
  vec<uint64_t> simp_coeffs;
  lits.copyTo(simp_lits);
  coeffs.copyTo(simp_coeffs);

  lits.clear();
  coeffs.clear();

  // Fix literals that have a coeff larger than rhs.
  for (int i = 0; i < simp_lits.size(); i++) {
    if (simp_coeffs[i] == 0)
      continue;
	
    // CHANGED TO INT64_MAX for now - Sukrut
    if (simp_coeffs[i] >= INT64_MAX) {
      printf("c Overflow in the Encoding\n");
      printf("s UNKNOWN\n");
      exit(_ERROR_);
    }

    if (simp_coeffs[i] <= (unsigned)rhs || 
          incremental_strategy == _INCREMENTAL_ITERATIVE_) {
      lits.push(simp_lits[i]);
      coeffs.push(simp_coeffs[i]);
    } else
      addUnitClause(S, ~simp_lits[i]);
  }

  if (lits.size() == 1) { // TODO - how is this sound? - Sukrut
    // addUnitClause(S, ~lits[0]);
    return;
  }

  if (lits.size() == 0)
    return;

  weightedlitst iliterals;
  for (int i = 0; i < lits.size(); i++) {
    wlitt wl;
    wl.lit = lits[i];
    wl.weight = coeffs[i];
    iliterals.push_back(wl);
  }
  less_than_wlitt lt_wlit;
  std::sort(iliterals.begin(), iliterals.end(), lt_wlit);
  if(incremental_strategy == _INCREMENTAL_ITERATIVE_) {
    encodeLeqIncremental(rhs + 1, S, iliterals, pb_oliterals);
  } else {
    encodeLeq(rhs + 1, S, iliterals, pb_oliterals);
  }
  
  // fill up assumptions outside, like in totalizer encoding 
  // this function need not handle it
  if(incremental_strategy == _INCREMENTAL_NONE_) {
    for (wlit_mapt::reverse_iterator rit = pb_oliterals.rbegin();
      rit != pb_oliterals.rend(); rit++) {
      if (rit->first > rhs) {
      //  assumptions.push(~rit->second);
        addUnitClause(S, ~rit->second);
      } else {
        break;
      }
    } 
  }	
}

void GTEIncremental::encode(Solver *S, vec<Lit> &lits, vec<uint64_t> &coeffs,
                 uint64_t rhs) {
  // FIXME: do not change coeffs in this method. Make coeffs const.

  // If the rhs is larger than INT32_MAX is not feasible to encode this
  // pseudo-Boolean constraint to CNF.
  // CHANGED TO INT64_MAX for now - Sukrut
  if (rhs >= INT64_MAX) {
    printf("c Overflow in the Encoding\n");
    printf("s UNKNOWN\n");
    exit(_ERROR_);
  }

  hasEncoding = false; // TODO, Sukrut - what is the purpose of this?
  nb_variables = 0;
  nb_clauses = 0;
  
  build(S, lits, coeffs, rhs);
  
  for (int i = 0; i < lits.size(); i++) {
    wlitt wl;
    wl.lit = lits[i];
    wl.weight = coeffs[i];
    enc_literals.push_back(wl);
  }

  // addUnitClause(S,~pb_oliterals.rbegin()->second);
  /*
  if (pb_oliterals.rbegin()->first != rhs+1){
        printf("%d - %d\n",pb_oliterals.rbegin()->first,rhs);
        for(wlit_mapt::reverse_iterator
  rit=pb_oliterals.rbegin();rit!=pb_oliterals.rend();rit++)
  {
        printf("rit->first %d\n",rit->first);
  }
  }
  */
  // assert (pb_oliterals.rbegin()->first == rhs+1);
  // printLit(~pb_oliterals.rbegin()->second);
  /* ... PUT CODE HERE FOR CREATING THE ENCODING ... */
  /* ... do not forget to sort the coefficients so that GTE is more efficient
   * ... */

  current_pb_rhs = rhs;
  hasEncoding = true;
}

void GTEIncremental::update(Solver *S, uint64_t rhs, vec<Lit> &assumptions) {

  // TODO - for now, I am assuming that RHS does not increase in a given tree
  // when incremental is not involved
  assert(hasEncoding);
  if (rhs >= INT64_MAX) {
    printf("c Overflow in the Encoding\n");
    printf("s UNKNOWN\n");
    exit(_ERROR_);
  }
  if(incremental_strategy == _INCREMENTAL_NONE_) {
    for (wlit_mapt::reverse_iterator rit = pb_oliterals.rbegin();
         rit != pb_oliterals.rend(); rit++) {
      if (rit->first > current_pb_rhs)
        continue;
      if (rit->first > rhs) {
        addUnitClause(S, ~rit->second);
      } else {
        break;
      }
    }
  }
  /* ... PUT CODE HERE TO UPDATE THE RHS OF AN ALREADY EXISTING ENCODING ... */
  
  if(incremental_strategy == _INCREMENTAL_ITERATIVE_) {
    if(rhs > current_pb_rhs) {
//      pb_oliterals.clear();
      incremental(S, rhs);
// TODO      encodeLeqIncremental(rhs, S, enc_literals, pb_oliterals, 
//                      current_pb_rhs);
    }
    assumptions.clear();
    printf("Adding assumptions\n");
    for(wlit_mapt::reverse_iterator oit = ++(pb_oliterals.rbegin());
      oit != pb_oliterals.rend(); oit++) {
      if(oit->first > rhs) {
        printf("Assumption for weight %ld\n", oit->first);
        assumptions.push(~oit->second);
      } else {
        printf("No assumption for weight %ld\n", oit->first);
      }
    }
  }
  
  // add missing literals and clauses
  current_pb_rhs = rhs;
}

void GTEIncremental::join(Solver *S, vec<Lit> &lits, vec<uint64_t> &coeffs,
                 uint64_t rhs, vec<Lit> &assumptions) {
  
  assert(hasEncoding);	
  assert(incremental_strategy == _INCREMENTAL_ITERATIVE_);
// uint64_t old_pb = current_pb_rhs;
  
  // add extra clauses in existing tree
//  update(S, rhs, assumptions);
  
  wlit_mapt loutputs;
  wlit_mapt routputs;
  
  // backup literals in left tree
  loutputs.insert(pb_oliterals.begin(), pb_oliterals.end());
  pb_oliterals.clear();
  
  // build right tree
  build(S, lits, coeffs, rhs);

  // backup literals in right tree
  routputs.insert(pb_oliterals.begin(), pb_oliterals.end());
  pb_oliterals.clear();
  
  // code copied from encodeLeqIncremental, create one new level in tree
  bool added_first_above_k = false;
  uint64_t least_weight_above_k = 0;
  
  assert(!loutputs.empty());
  assert(!routputs.empty());
  
  uint64_t left_largest_weight = loutputs.rbegin()->first;
  uint64_t right_largest_weight = routputs.rbegin()->first;
  
  // find last element for current level, if it directly comes from the 
  // level immediately below
  if(left_largest_weight > rhs) {
    added_first_above_k = true;
    if(right_largest_weight > rhs && 
          right_largest_weight < left_largest_weight) {
      least_weight_above_k = right_largest_weight;
    } else {
      least_weight_above_k = left_largest_weight;
    }
  } else if(right_largest_weight > rhs) {
    added_first_above_k = true;
    least_weight_above_k = right_largest_weight;
  }

  {
    
    for (wlit_mapt::iterator mit = loutputs.begin(); mit != loutputs.end();
         mit++) {

      if (mit->first > rhs) {
        assert(added_first_above_k);
        addBinaryClause(S, ~mit->second, 
          get_var(S, pb_oliterals, least_weight_above_k));
        nb_clauses++;
      } else {
        addBinaryClause(S, ~mit->second, get_var(S, pb_oliterals, mit->first));
        nb_clauses++;
        // clause.push_back(get_var(auxvars,oliterals,l.first));
      }
      // formula.push_back(std::move(clause));
    }
  }

  {
    
    for (wlit_mapt::iterator mit = routputs.begin(); mit != routputs.end();
         mit++) {

      if (mit->first > rhs) {
        assert(added_first_above_k);
        addBinaryClause(S, ~mit->second,
          get_var(S, pb_oliterals, least_weight_above_k));
        nb_clauses++;
        // clause.push_back(get_var(auxvars,oliterals,k));
      } else {
        addBinaryClause(S, ~mit->second, get_var(S, pb_oliterals, mit->first));
        nb_clauses++;
        // clause.push_back(get_var(auxvars,oliterals,r.first));
      }

      // formula.push_back(std::move(clause));
    }
  }
  
  // finding least weight above k among pairwise sums, since we already 
  // know that no single weight is above k
  if(!added_first_above_k) {
    least_weight_above_k = loutputs.rbegin()->first + routputs.rbegin()->first;
    if(least_weight_above_k > rhs) {
      added_first_above_k = true;
      for (wlit_mapt::iterator lit = loutputs.begin(); lit != loutputs.end();
         lit++) {
        for (wlit_mapt::iterator rit = routputs.begin(); rit != routputs.end();
           rit++) {
          uint64_t tw = lit->first + rit->first;
          if(tw > rhs && tw < least_weight_above_k) {
            least_weight_above_k = tw;
          }
        }
      }
    }
  }

  // if(!lformula.empty() && !rformula.empty())
  {
    // sending pairwise sums to parent
    for (wlit_mapt::iterator lit = loutputs.begin(); lit != loutputs.end();
         lit++) {
      for (wlit_mapt::iterator rit = routputs.begin(); rit != routputs.end();
           rit++) {
        /*clauset clause;
        clause.push_back(-l.second);
        clause.push_back(-r.second);*/
        uint64_t tw = lit->first + rit->first;
        if (tw > rhs) {
          assert(added_first_above_k);
          addTernaryClause(S, ~lit->second, ~rit->second,
                           get_var(S, pb_oliterals, least_weight_above_k)); // TODO - check
          nb_clauses++;
          // clause.push_back(get_var(auxvars,oliterals,k));
        } else {
          addTernaryClause(S, ~lit->second, ~rit->second,
                           get_var(S, pb_oliterals, tw));
          nb_clauses++;
          // clause.push_back(get_var(auxvars,oliterals,tw));
        }

        // formula.push_back(std::move(clause));
      }
    }
  }
  
  // TODO - check if needed
/*  less_than_map sort_map;
  std::sort(pb_oliterals.begin(), pb_oliterals.end(), sort_map); */
  
  for(wlit_mapt::reverse_iterator oit = ++(pb_oliterals.rbegin());
      oit != pb_oliterals.rend(); oit++) {
    wlit_mapt::reverse_iterator implied_lit = oit;
    --implied_lit;
    addBinaryClause(S, ~oit->second, implied_lit->second);
    nb_clauses++;
  } 
  
//  gteIterative_left.push();
//  new (&gteIterative_left[gteIterative_left.size() - 1])
//      weightedlitst();
//  gteIterative_right.push();
//  new (&gteIterative_right[gteIterative_right.size() - 1])
//      weightedlitst();
//  gteIterative_output.push();  
//  new (&gteIterative_output[gteIterative_output.size() - 1])
//      weightedlitst();
//      
//  for(wlit_mapt::iterator lit = loutputs.begin(); lit != loutputs.end();
//         lit++) {
//    gteIterative_left[gteIterative_left.size() - 1].emplace_back(
//      wlitt{lit->second, lit->first});
//  }
//  
//  for(wlit_mapt::iterator rit = routputs.begin(); rit != routputs.end();
//        rit++) {
//    gteIterative_right[gteIterative_right.size() - 1].emplace_back(
//      wlitt{rit->second, rit->first});
//  }
//  
//  for(wlit_mapt::iterator oit = pb_oliterals.begin(); oit != pb_oliterals.end();
//         oit++) {
//    gteIterative_output[gteIterative_output.size() - 1].emplace_back(
//      wlitt{oit->second, oit->first});
//  }
//  
//  current_pb_rhs = rhs;
//  gteIterative_rhs.push(current_pb_rhs);

  incremental(S, rhs);

  adder(loutputs, routputs, pb_oliterals, rhs);
  current_pb_rhs = rhs;
  
  pb_oliterals.clear();
  pb_oliterals.insert(gteIterative_output[gteIterative_output.size()-1].begin(),
    gteIterative_output[gteIterative_output.size()-1].end());
  
}

void GTEIncremental::adder(wlit_mapt &left, wlit_mapt &right,
                            wlit_mapt &output, uint64_t rhs) {
  gteIterative_left.push();
  new (&gteIterative_left[gteIterative_left.size() - 1])
      wlit_mapt();
  gteIterative_right.push();
  new (&gteIterative_right[gteIterative_right.size() - 1])
      wlit_mapt();
  gteIterative_output.push();  
  new (&gteIterative_output[gteIterative_output.size() - 1])
      wlit_mapt();
      
  for(wlit_mapt::iterator lit = left.begin(); lit != left.end();
         lit++) {
    gteIterative_left[gteIterative_left.size() - 1][lit->first] = lit->second;
  }
  
  for(wlit_mapt::iterator rit = right.begin(); rit != right.end();
        rit++) {
    gteIterative_right[gteIterative_right.size() - 1][rit->first] = rit->second;
  }
  
  for(wlit_mapt::iterator oit = output.begin(); oit != output.end();
         oit++) {
    gteIterative_output[gteIterative_output.size()-1][oit->first] = oit->second;
  }
  
//  current_pb_rhs = rhs;
  gteIterative_rhs.push(current_pb_rhs);                            
}

void GTEIncremental::incremental(Solver *S, uint64_t rhs) {
  
  for (int z = 0; z < gteIterative_rhs.size(); z++) {
        
    bool added_first_above_k = false;
    uint64_t least_weight_above_k = 0;

    assert(!gteIterative_left[z].empty());
    assert(!gteIterative_right[z].empty());

    uint64_t left_largest_weight = gteIterative_left[z].rbegin()->first;
    uint64_t right_largest_weight = gteIterative_right[z].rbegin()->first;

    // find last element for current level, if it directly comes from the 
    // level immediately below
    if(left_largest_weight > rhs) {
      added_first_above_k = true;
      if(right_largest_weight > rhs && 
            right_largest_weight < left_largest_weight) {
        least_weight_above_k = right_largest_weight;
      } else {
        least_weight_above_k = left_largest_weight;
      }
    } else if(right_largest_weight > rhs) {
      added_first_above_k = true;
      least_weight_above_k = right_largest_weight;
    }
    
    uint64_t old_least_above_k = gteIterative_output[z].rbegin()->first;
    
    if(added_first_above_k && least_weight_above_k > old_least_above_k) {
      
      for(wlit_mapt::iterator lit = gteIterative_left[z].begin(); 
          lit != gteIterative_left[z].end(); lit++) {

        if (lit->first > rhs) {
          assert(added_first_above_k);
          addBinaryClause(S, ~lit->second, 
            get_var(S, gteIterative_output[z], least_weight_above_k));
          nb_clauses++;
        } else if(lit->first > gteIterative_rhs[z]) {
            addBinaryClause(S, ~lit->second, get_var(S, gteIterative_output[z],
              lit->first));
            nb_clauses++;
            // clause.push_back(get_var(auxvars,oliterals,l.first));
        }     
      } 
      
      for(wlit_mapt::iterator rit = gteIterative_right[z].begin(); 
          rit != gteIterative_right[z].end(); rit++) {
        if (rit->first > rhs) {
          assert(added_first_above_k);
          addBinaryClause(S, ~rit->second, 
            get_var(S, gteIterative_output[z], least_weight_above_k));
          nb_clauses++;
        } else if(rit->first > gteIterative_rhs[z]) {
            addBinaryClause(S, ~rit->second, get_var(S, gteIterative_output[z], 
              rit->first));
            nb_clauses++;
            // clause.push_back(get_var(auxvars,oliterals,l.first));
        }     
      } 
    }
    
    if(!added_first_above_k) {
    least_weight_above_k = gteIterative_left[z].rbegin()->first + 
                            gteIterative_right[z].rbegin()->first;
      if(least_weight_above_k > rhs) {
        added_first_above_k = true;
        for (wlit_mapt::iterator lit = gteIterative_left[z].begin();
            lit != gteIterative_left[z].end(); lit++) {
          for (wlit_mapt::iterator rit = gteIterative_right[z].begin();
            rit != gteIterative_right[z].end(); rit++) {
            uint64_t tw = lit->first + rit->first;
            if(tw > rhs && tw < least_weight_above_k) {
              least_weight_above_k = tw;
            }
          }
        }
      }
    }
    
    if(added_first_above_k && least_weight_above_k > old_least_above_k) {
      
      for (wlit_mapt::iterator lit = gteIterative_left[z].begin();
            lit != gteIterative_left[z].end(); lit++) {
        for (wlit_mapt::iterator rit = gteIterative_right[z].begin(); 
              rit != gteIterative_right[z].end(); rit++) {
          /*clauset clause;
          clause.push_back(-l.second);
          clause.push_back(-r.second);*/
          uint64_t tw = lit->first + rit->first;
          if (tw > rhs) {
            assert(added_first_above_k);
            addTernaryClause(S, ~lit->second, ~rit->second,
                             get_var(S, pb_oliterals, least_weight_above_k)); // TODO - check
            nb_clauses++;
            // clause.push_back(get_var(auxvars,oliterals,k));
          } else if(tw > gteIterative_rhs[z]) {
            addTernaryClause(S, ~lit->second, ~rit->second,
                             get_var(S, pb_oliterals, tw));
            nb_clauses++;
            // clause.push_back(get_var(auxvars,oliterals,tw));
          }

          // formula.push_back(std::move(clause));
        }
      }  
      
    }
    
    for(wlit_mapt::reverse_iterator oit = ++(gteIterative_output[z].rbegin());
      oit != gteIterative_output[z].rend(); oit++) {
      if(oit->first <= gteIterative_rhs[z]) {
        break;
      }
      wlit_mapt::reverse_iterator implied_lit = oit;
      --implied_lit;
      addBinaryClause(S, ~oit->second, implied_lit->second);
      nb_clauses++;
    }  
    
    gteIterative_rhs[z] = rhs;   
        
  }
  
  current_pb_rhs = rhs;
  pb_oliterals.clear();
  pb_oliterals.insert(gteIterative_output[gteIterative_output.size()-1].begin(),
    gteIterative_output[gteIterative_output.size()-1].end());
  
  
}
