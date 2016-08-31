/*
 * Copyright 2016 The Netty Project
 *
 * The Netty Project licenses this file to you under the Apache License,
 * version 2.0 (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at:
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
/* Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef TCN_H
#define TCN_H

// Start includes
#include <jni.h>

#include "apr.h"
#include "apr_pools.h"

#ifndef APR_HAS_THREADS
#error "Missing APR_HAS_THREADS support from APR."
#endif

#include <stdio.h>
#include <stdlib.h>
#if defined(_WIN32) && !defined(__CYGWIN__)
#include <process.h>
#else
#include <unistd.h>
#endif

#if defined(_DEBUG) || defined(DEBUG)
#include <assert.h>
#define TCN_ASSERT(x)  assert((x))
#else
#define TCN_ASSERT(x) (void)0
#endif
// End includes

#define TCN_CLASS_PATH  "io/netty/tcnative/jni/"
#define TCN_PARENT_IDE  "TCN_PARENT_ID"

#define UNREFERENCED(P)      (P) = (P)
#define UNREFERENCED_STDARGS e = e; o = o
#ifdef WIN32
#define LLT(X) (X)
#else
#define LLT(X) ((long)(X))
#endif
#define P2J(P)          ((jlong)LLT(P))
#define J2P(P, T)       ((T)LLT((jlong)P))
/* On stack buffer size */
#define TCN_BUFFER_SZ   8192
#define TCN_STDARGS     JNIEnv *e, jobject o

#define TCN_IMPLEMENT_CALL(RT, CL, FN)  \
    JNIEXPORT RT JNICALL Java_io_netty_tcnative_jni_##CL##_##FN

#define TCN_IMPLEMENT_METHOD(RT, FN)    \
    static RT method_##FN


/* Private helper functions */
void            tcn_Throw(JNIEnv *, const char *, ...);
void            tcn_ThrowException(JNIEnv *, const char *);
void            tcn_ThrowAPRException(JNIEnv *, apr_status_t);
jstring         tcn_new_string(JNIEnv *, const char *);
jstring         tcn_new_stringn(JNIEnv *, const char *, size_t);
jbyteArray      tcn_new_arrayb(JNIEnv *, const unsigned char *, size_t);
jobjectArray    tcn_new_arrays(JNIEnv *env, size_t len);
char           *tcn_get_string(JNIEnv *, jstring);
char           *tcn_strdup(JNIEnv *, jstring);
char           *tcn_pstrdup(JNIEnv *, jstring, apr_pool_t *);

#define J2S(V)  c##V
#define J2L(V)  p##V

#define TCN_BEGIN_MACRO     if (1) {
#define TCN_END_MACRO       } else (void)(0)

#define TCN_ALLOC_CSTRING(V)     \
    const char *c##V = V ? (const char *)((*e)->GetStringUTFChars(e, V, 0)) : NULL

#define TCN_FREE_CSTRING(V)      \
    if (c##V) (*e)->ReleaseStringUTFChars(e, V, c##V)

#define TCN_ALLOC_JSTRING(V)     \
    char *c##V = tcn_get_string(e, (V))

#define AJP_TO_JSTRING(V)   (*e)->NewStringUTF((e), (V))

#define TCN_FREE_JSTRING(V)      \
    TCN_BEGIN_MACRO              \
        if (c##V)                \
            free(c##V);          \
    TCN_END_MACRO

#define TCN_THROW_IF_ERR(x, r)                  \
    TCN_BEGIN_MACRO                             \
        apr_status_t R = (x);                   \
        if (R != APR_SUCCESS) {                 \
            tcn_ThrowAPRException(e, R);        \
            (r) = 0;                            \
            goto cleanup;                       \
        }                                       \
    TCN_END_MACRO

#define TCN_THROW_OS_ERROR(E)   \
    tcn_ThrowAPRException((E), apr_get_os_error())

#define TCN_LOAD_CLASS(E, C, N, R)                  \
    TCN_BEGIN_MACRO                                 \
        jclass _##C = (*(E))->FindClass((E), N);    \
        if (_##C == NULL) {                         \
            (*(E))->ExceptionClear((E));            \
            return R;                               \
        }                                           \
        C = (*(E))->NewGlobalRef((E), _##C);        \
        (*(E))->DeleteLocalRef((E), _##C);          \
    TCN_END_MACRO

#define TCN_UNLOAD_CLASS(E, C)                      \
        (*(E))->DeleteGlobalRef((E), (C))

#define TCN_IS_NULL(E, O)                           \
        ((*(E))->IsSameObject((E), (O), NULL) == JNI_TRUE)

#define TCN_GET_METHOD(E, C, M, N, S, R)            \
    TCN_BEGIN_MACRO                                 \
        M = (*(E))->GetMethodID((E), C, N, S);      \
        if (M == NULL) {                            \
            return R;                               \
        }                                           \
    TCN_END_MACRO

#define TCN_GET_FIELD(E, C, F, N, S, R)            \
    TCN_BEGIN_MACRO                                 \
        F = (*(E))->GetFieldID((E), C, N, S);      \
        if (F == NULL) {                            \
            return R;                               \
        }                                           \
    TCN_END_MACRO

#define TCN_MAX_METHODS 8

#define TCN_MIN(a, b) ((a) < (b) ? (a) : (b))
#define TCN_MAX(a, b) ((a) > (b) ? (a) : (b))

#ifdef WIN32
#define TCN_ALLOC_WSTRING(V)     \
    jsize wl##V = (*e)->GetStringLength(e, V);   \
    const jchar *ws##V = V ? (const jchar *)((*e)->GetStringChars(e, V, 0)) : NULL; \
    jchar *w##V = NULL

#define TCN_INIT_WSTRING(V)                                     \
        w##V = (jchar *)malloc((wl##V + 1) * sizeof(jchar));    \
        wcsncpy(w##V, ws##V, wl##V);                        \
        w##V[wl##V] = 0

#define TCN_FREE_WSTRING(V)      \
    if (ws##V) (*e)->ReleaseStringChars(e, V, ws##V); \
    if (ws##V) free (w##V)

#define J2W(V)  w##V

#endif


/* Return global apr pool
 */
apr_pool_t *tcn_get_global_pool(void);

/* Return global String class
 */
jclass tcn_get_string_class(void);

jclass tcn_get_byte_array_class();
jfieldID tcn_get_key_material_certificate_chain_field();
jfieldID tcn_get_key_material_private_key_field();

/* Return global JVM initalized on JNI_OnLoad
 */
JavaVM *tcn_get_java_vm(void);

/* Get current thread JNIEnv
 */
jint tcn_get_java_env(JNIEnv **);

#endif /* TCN_H */
