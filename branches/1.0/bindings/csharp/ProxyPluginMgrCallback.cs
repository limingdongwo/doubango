/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.39
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

namespace org.doubango.tinyWRAP {

using System;
using System.Runtime.InteropServices;

public class ProxyPluginMgrCallback : IDisposable {
  private HandleRef swigCPtr;
  protected bool swigCMemOwn;

  internal ProxyPluginMgrCallback(IntPtr cPtr, bool cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(ProxyPluginMgrCallback obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~ProxyPluginMgrCallback() {
    Dispose();
  }

  public virtual void Dispose() {
    lock(this) {
      if(swigCPtr.Handle != IntPtr.Zero && swigCMemOwn) {
        swigCMemOwn = false;
        tinyWRAPPINVOKE.delete_ProxyPluginMgrCallback(swigCPtr);
      }
      swigCPtr = new HandleRef(null, IntPtr.Zero);
      GC.SuppressFinalize(this);
    }
  }

  public ProxyPluginMgrCallback() : this(tinyWRAPPINVOKE.new_ProxyPluginMgrCallback(), true) {
    SwigDirectorConnect();
  }

  public virtual int OnPluginCreated(ulong id, twrap_proxy_plugin_type_t type) {
    int ret = ((this.GetType() == typeof(ProxyPluginMgrCallback)) ? tinyWRAPPINVOKE.ProxyPluginMgrCallback_OnPluginCreated(swigCPtr, id, (int)type) : tinyWRAPPINVOKE.ProxyPluginMgrCallback_OnPluginCreatedSwigExplicitProxyPluginMgrCallback(swigCPtr, id, (int)type));
    return ret;
  }

  public virtual int OnPluginDestroyed(ulong id, twrap_proxy_plugin_type_t type) {
    int ret = ((this.GetType() == typeof(ProxyPluginMgrCallback)) ? tinyWRAPPINVOKE.ProxyPluginMgrCallback_OnPluginDestroyed(swigCPtr, id, (int)type) : tinyWRAPPINVOKE.ProxyPluginMgrCallback_OnPluginDestroyedSwigExplicitProxyPluginMgrCallback(swigCPtr, id, (int)type));
    return ret;
  }

  private void SwigDirectorConnect() {
    if (SwigDerivedClassHasMethod("OnPluginCreated", swigMethodTypes0))
      swigDelegate0 = new SwigDelegateProxyPluginMgrCallback_0(SwigDirectorOnPluginCreated);
    if (SwigDerivedClassHasMethod("OnPluginDestroyed", swigMethodTypes1))
      swigDelegate1 = new SwigDelegateProxyPluginMgrCallback_1(SwigDirectorOnPluginDestroyed);
    tinyWRAPPINVOKE.ProxyPluginMgrCallback_director_connect(swigCPtr, swigDelegate0, swigDelegate1);
  }

  private bool SwigDerivedClassHasMethod(string methodName, Type[] methodTypes) {
    System.Reflection.MethodInfo methodInfo = this.GetType().GetMethod(methodName, System.Reflection.BindingFlags.Public | System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Instance, null, methodTypes, null);
    bool hasDerivedMethod = methodInfo.DeclaringType.IsSubclassOf(typeof(ProxyPluginMgrCallback));
    return hasDerivedMethod;
  }

  private int SwigDirectorOnPluginCreated(ulong id, int type) {
    return OnPluginCreated(id, (twrap_proxy_plugin_type_t)type);
  }

  private int SwigDirectorOnPluginDestroyed(ulong id, int type) {
    return OnPluginDestroyed(id, (twrap_proxy_plugin_type_t)type);
  }

  public delegate int SwigDelegateProxyPluginMgrCallback_0(ulong id, int type);
  public delegate int SwigDelegateProxyPluginMgrCallback_1(ulong id, int type);

  private SwigDelegateProxyPluginMgrCallback_0 swigDelegate0;
  private SwigDelegateProxyPluginMgrCallback_1 swigDelegate1;

  private static Type[] swigMethodTypes0 = new Type[] { typeof(ulong), typeof(twrap_proxy_plugin_type_t) };
  private static Type[] swigMethodTypes1 = new Type[] { typeof(ulong), typeof(twrap_proxy_plugin_type_t) };
}

}
