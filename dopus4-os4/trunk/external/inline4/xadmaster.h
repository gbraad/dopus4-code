#ifndef INLINE4_XADMASTER_H
#define INLINE4_XADMASTER_H

/*
** This file was auto generated by idltool 52.7.
**
** It provides compatibility to OS3 style library
** calls by substituting functions.
**
** Do not edit manually.
*/ 

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef EXEC_EXEC_H
#include <exec/exec.h>
#endif
#ifndef EXEC_INTERFACES_H
#include <exec/interfaces.h>
#endif

#ifndef UTILITY_TAGITEM_H
#include <utility/tagitem.h>
#endif
#ifndef LIBRARIES_XADMASTER_H
#include <libraries/xadmaster.h>
#endif
#include <interfaces/xadmaster.h>

/* Inline macros for Interface "main" */
#define xadAllocObjectA(type, tags) IXadMaster->xadAllocObjectA((type), (tags)) 
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (__GNUC__ >= 3)
#define xadAllocObject(...) IXadMaster->xadAllocObject(__VA_ARGS__) 
#elif (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#define xadAllocObject(vargs...) IXadMaster->xadAllocObject(## vargs) 
#endif
#define xadFreeObjectA(object, tags) IXadMaster->xadFreeObjectA((object), (tags)) 
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (__GNUC__ >= 3)
#define xadFreeObject(...) IXadMaster->xadFreeObject(__VA_ARGS__) 
#elif (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#define xadFreeObject(vargs...) IXadMaster->xadFreeObject(## vargs) 
#endif
#define xadRecogFileA(size, memory, tags) IXadMaster->xadRecogFileA((size), (memory), (tags)) 
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (__GNUC__ >= 3)
#define xadRecogFile(size, ...) IXadMaster->xadRecogFile((size), __VA_ARGS__) 
#elif (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#define xadRecogFile(size, vargs...) IXadMaster->xadRecogFile(size, ## vargs) 
#endif
#define xadGetInfoA(ai, tags) IXadMaster->xadGetInfoA((ai), (tags)) 
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (__GNUC__ >= 3)
#define xadGetInfo(...) IXadMaster->xadGetInfo(__VA_ARGS__) 
#elif (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#define xadGetInfo(vargs...) IXadMaster->xadGetInfo(## vargs) 
#endif
#define xadFreeInfo(ai) IXadMaster->xadFreeInfo((ai)) 
#define xadFileUnArcA(ai, tags) IXadMaster->xadFileUnArcA((ai), (tags)) 
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (__GNUC__ >= 3)
#define xadFileUnArc(...) IXadMaster->xadFileUnArc(__VA_ARGS__) 
#elif (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#define xadFileUnArc(vargs...) IXadMaster->xadFileUnArc(## vargs) 
#endif
#define xadDiskUnArcA(ai, tags) IXadMaster->xadDiskUnArcA((ai), (tags)) 
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (__GNUC__ >= 3)
#define xadDiskUnArc(...) IXadMaster->xadDiskUnArc(__VA_ARGS__) 
#elif (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#define xadDiskUnArc(vargs...) IXadMaster->xadDiskUnArc(## vargs) 
#endif
#define xadGetErrorText(errnum) IXadMaster->xadGetErrorText((errnum)) 
#define xadGetClientInfo() IXadMaster->xadGetClientInfo() 
#define xadHookAccess(command, data, buffer, ai) IXadMaster->xadHookAccess((command), (data), (buffer), (ai)) 
#define xadConvertDatesA(tags) IXadMaster->xadConvertDatesA((tags)) 
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (__GNUC__ >= 3)
#define xadConvertDates(...) IXadMaster->xadConvertDates(__VA_ARGS__) 
#elif (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#define xadConvertDates(...) IXadMaster->xadConvertDates(## vargs) 
#endif
#define xadCalcCRC16(id, init, size, buffer) IXadMaster->xadCalcCRC16((id), (init), (size), (buffer)) 
#define xadCalcCRC32(id, init, size, buffer) IXadMaster->xadCalcCRC32((id), (init), (size), (buffer)) 
#define xadAllocVec(size, flags) IXadMaster->xadAllocVec((size), (flags)) 
#define xadCopyMem(src, dest, size) IXadMaster->xadCopyMem((src), (dest), (size)) 
#define xadHookTagAccessA(command, data, buffer, ai, tags) IXadMaster->xadHookTagAccessA((command), (data), (buffer), (ai), (tags)) 
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (__GNUC__ >= 3)
#define xadHookTagAccess(command, data, buffer, ...) IXadMaster->xadHookTagAccess((command), (data), (buffer), __VA_ARGS__) 
#elif (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#define xadHookTagAccess(command, data, buffer, vargs...) IXadMaster->xadHookTagAccess(command, data, buffer, ## vargs) 
#endif
#define xadConvertProtectionA(tags) IXadMaster->xadConvertProtectionA((tags)) 
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (__GNUC__ >= 3)
#define xadConvertProtection(...) IXadMaster->xadConvertProtection(__VA_ARGS__) 
#elif (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#define xadConvertProtection(...) IXadMaster->xadConvertProtection(## vargs) 
#endif
#define xadGetDiskInfoA(ai, tags) IXadMaster->xadGetDiskInfoA((ai), (tags)) 
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (__GNUC__ >= 3)
#define xadGetDiskInfo(...) IXadMaster->xadGetDiskInfo(__VA_ARGS__) 
#elif (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#define xadGetDiskInfo(vargs...) IXadMaster->xadGetDiskInfo(## vargs) 
#endif
#define xadDiskFileUnArcA(ai, tags) IXadMaster->xadDiskFileUnArcA((ai), (tags)) 
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (__GNUC__ >= 3)
#define xadDiskFileUnArc(...) IXadMaster->xadDiskFileUnArc(__VA_ARGS__) 
#elif (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#define xadDiskFileUnArc(vargs...) IXadMaster->xadDiskFileUnArc(## vargs) 
#endif
#define xadGetHookAccessA(ai, tags) IXadMaster->xadGetHookAccessA((ai), (tags)) 
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (__GNUC__ >= 3)
#define xadGetHookAccess(...) IXadMaster->xadGetHookAccess(__VA_ARGS__) 
#elif (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#define xadGetHookAccess(vargs...) IXadMaster->xadGetHookAccess(## vargs) 
#endif
#define xadFreeHookAccessA(ai, tags) IXadMaster->xadFreeHookAccessA((ai), (tags)) 
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (__GNUC__ >= 3)
#define xadFreeHookAccess(...) IXadMaster->xadFreeHookAccess(__VA_ARGS__) 
#elif (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#define xadFreeHookAccess(vargs...) IXadMaster->xadFreeHookAccess(## vargs) 
#endif
#define xadAddFileEntryA(fi, ai, tags) IXadMaster->xadAddFileEntryA((fi), (ai), (tags)) 
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (__GNUC__ >= 3)
#define xadAddFileEntry(fi, ...) IXadMaster->xadAddFileEntry((fi), __VA_ARGS__) 
#elif (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#define xadAddFileEntry(fi, vargs...) IXadMaster->xadAddFileEntry(fi, ## vargs) 
#endif
#define xadAddDiskEntryA(di, ai, tags) IXadMaster->xadAddDiskEntryA((di), (ai), (tags)) 
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (__GNUC__ >= 3)
#define xadAddDiskEntry(di, ...) IXadMaster->xadAddDiskEntry((di), __VA_ARGS__) 
#elif (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#define xadAddDiskEntry(di, vargs...) IXadMaster->xadAddDiskEntry(di, ## vargs) 
#endif
#define xadGetFilenameA(buffersize, buffer, path, name, tags) IXadMaster->xadGetFilenameA((buffersize), (buffer), (path), (name), (tags)) 
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (__GNUC__ >= 3)
#define xadGetFilename(buffersize, buffer, path, ...) IXadMaster->xadGetFilename((buffersize), (buffer), (path), __VA_ARGS__) 
#elif (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#define xadGetFilename(buffersize, buffer, path, vargs...) IXadMaster->xadGetFilename(buffersize, buffer, path, ## vargs) 
#endif
#define xadConvertNameA(charset, tags) IXadMaster->xadConvertNameA((charset), (tags)) 
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (__GNUC__ >= 3)
#define xadConvertName(...) IXadMaster->xadConvertName(__VA_ARGS__) 
#elif (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#define xadConvertName(vargs...) IXadMaster->xadConvertName(## vargs) 
#endif
#define xadGetDefaultNameA(tags) IXadMaster->xadGetDefaultNameA((tags)) 
#if (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || (__GNUC__ >= 3)
#define xadGetDefaultName(...) IXadMaster->xadGetDefaultName(__VA_ARGS__) 
#elif (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#define xadGetDefaultName(...) IXadMaster->xadGetDefaultName(## vargs) 
#endif
#define xadGetSystemInfo() IXadMaster->xadGetSystemInfo() 

#endif /* INLINE4_XADMASTER_H */
