#if !defined(SingletonMode_define)
#define SingletonMode_define

template <typename T>
  class   SingletonMode
  {   
  public:   
      static T * getInstance(){
		  //if(!ptr_){
		  if(0 == m_Counter){
			  m_Object = new T();   
		  }
		  ++m_Counter;   
		  return m_Object;   
	  }   
      static   void   releaseInstance(T * _p){
		  if(0 == --m_Counter){
			  delete m_Object;  
			  m_Object = 0;
		  }
          if (m_Counter < 0)
          {
              m_Counter = 0;
          }
	  }
	  static T * getInstance(HWND hwnd){
		  //if (!ptr_)
		  if (0 == m_Counter)
		  {
			  m_Object = new T(hwnd);
		  }
		  ++m_Counter;
		  return m_Object;
	  }
  private:
      static T * m_Object;
      static size_t m_Counter;
  };   

#endif