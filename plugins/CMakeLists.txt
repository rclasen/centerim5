# When you add files here, also add them in po/POTFILES.in.
set(extaction_SOURCES
  extaction.c)

add_library(extaction SHARED ${extaction_SOURCES})

set_property(TARGET extaction PROPERTY
  PREFIX "")
target_include_directories(extaction PRIVATE
  ${Intl_INCLUDE_DIRS})
target_compile_options(extaction PRIVATE
  ${PURPLE_CFLAGS} ${GLIB2_CFLAGS})
target_link_libraries(extaction PRIVATE
  ${Intl_LIBRARIES} ${PURPLE_LDFLAGS} ${GLIB2_LDFLAGS})

install(TARGETS extaction DESTINATION lib/centerim5)
