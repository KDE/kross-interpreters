macro_optional_find_package(Ruby)

set(RUBY_COMPILED RUBY_LIBRARY AND RUBY_INCLUDE_DIRS)
macro_log_feature(RUBY_LIBRARY
  "Ruby" "Ruby libraries" "http://www.ruby-lang.org" FALSE "" "Needed to compile the Ruby bindings")

set(krossruby_PART_SRCS
   rubyvariant.cpp
   rubyinterpreter.cpp
   rubyextension.cpp
   rubyscript.cpp
   rubymodule.cpp
   rubycallcache.cpp
   rubyobject.cpp
   )
if(RUBY_COMPILED)
  include_directories(${RUBY_INCLUDE_DIRS})
  kde4_add_plugin(krossruby ${krossruby_PART_SRCS})
  target_link_libraries(krossruby ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${KDE4_KROSSCORE_LIBS} ${RUBY_LIBRARY})
  install(TARGETS krossruby  DESTINATION ${PLUGIN_INSTALL_DIR})
endif()