%include <std_common.i>

%{
#include <utility>
%}


namespace std {
  template <class T, class U > struct pair {      
    typedef T fisrt_type;
    typedef U second_type;
    
    %traits_swigtype(T);
    %traits_swigtype(U);
      
    %fragment(SWIG_Traits_frag(std::pair<T,U >), "header",
	      fragment=SWIG_Traits_frag(T),
	      fragment=SWIG_Traits_frag(U),
	      fragment="StdPairTraits") {
      namespace swig {
	template <>  struct traits<std::pair<T,U > > {
	  typedef pointer_category category;
	  static const char* type_name() {
	    return "std::pair<" #T "," #U " >";
	  }
	};
      }
    }

    %typemap_traits_ptr(SWIG_TYPECHECK_PAIR, std::pair<T,U >);

    pair();
    pair(T __a, U __b);
    pair(const pair& __p);

    T first;
    U second;

    %swig_pair_methods(std::pair<T,U >)
  };

  // ***
  // The following specializations should dissapear or get 
  // simplified when a 'const SWIGTYPE*&' can be defined
  // ***
  template <class T, class U > struct pair<T, U*> {      
    typedef T fisrt_type;
    typedef U* second_type;
    
    %traits_swigtype(T);
    %traits_swigtype(U);
      
    %fragment(SWIG_Traits_frag(std::pair<T,U* >), "header",
	      fragment=SWIG_Traits_frag(T),
	      fragment=SWIG_Traits_frag(U),
	      fragment="StdPairTraits") {
      namespace swig {
	template <>  struct traits<std::pair<T,U* > > {
	  typedef pointer_category category;
	  static const char* type_name() {
	    return "std::pair<" #T "," #U " * >";
	  }
	};
      }
    }

    %typemap_traits_ptr(SWIG_TYPECHECK_PAIR, std::pair<T,U* >);

    pair();
    pair(T __a, U* __b);
    pair(const pair& __p);

    T first;
    U* second;

    %swig_pair_methods(std::pair<T,U*>)
  };

  template <class T, class U > struct pair<T*, U> {      
    typedef T* fisrt_type;
    typedef U second_type;
    
    %traits_swigtype(T);
    %traits_swigtype(U);
      
    %fragment(SWIG_Traits_frag(std::pair<T*,U >), "header",
	      fragment=SWIG_Traits_frag(T),
	      fragment=SWIG_Traits_frag(U),
	      fragment="StdPairTraits") {
      namespace swig {
	template <>  struct traits<std::pair<T*,U > > {
	  typedef pointer_category category;
	  static const char* type_name() {
	    return "std::pair<" #T " *," #U " >";
	  }
	};
      }
    }

    %typemap_traits_ptr(SWIG_TYPECHECK_PAIR, std::pair<T*,U >);

    pair();
    pair(T* __a, U __b);
    pair(const pair& __p);

    T* first;
    U second;

    %swig_pair_methods(std::pair<T*,U >)
  };

  template <class T, class U > struct pair<T*, U*> {
    typedef T* fisrt_type;
    typedef U* second_type;

    %traits_swigtype(T);
    %traits_swigtype(U);
      
    %fragment(SWIG_Traits_frag(std::pair<T*,U* >), "header",
	      fragment=SWIG_Traits_frag(T),
	      fragment=SWIG_Traits_frag(U),
	      fragment="StdPairTraits") {
      namespace swig {
	template <>  struct traits<std::pair<T*,U* > > {
	  typedef pointer_category category;
	  static const char* type_name() {
	    return "std::pair<" #T " *," #U " * >";
	  }
	};
      }
    }

    %typemap_traits(SWIG_TYPECHECK_PAIR, std::pair<T*,U* >);

    pair();
    pair(T* __a, U* __b);
    pair(const pair& __p);

    T* first;
    U* second;

    %swig_pair_methods(std::pair<T*,U*>)
  };

}
