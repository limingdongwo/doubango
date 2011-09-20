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

public class MediaContentCPIM : MediaContent {
  private HandleRef swigCPtr;

  internal MediaContentCPIM(IntPtr cPtr, bool cMemoryOwn) : base(tinyWRAPPINVOKE.MediaContentCPIMUpcast(cPtr), cMemoryOwn) {
    swigCPtr = new HandleRef(this, cPtr);
  }

  internal static HandleRef getCPtr(MediaContentCPIM obj) {
    return (obj == null) ? new HandleRef(null, IntPtr.Zero) : obj.swigCPtr;
  }

  ~MediaContentCPIM() {
    Dispose();
  }

  public override void Dispose() {
    lock(this) {
      if(swigCPtr.Handle != IntPtr.Zero && swigCMemOwn) {
        swigCMemOwn = false;
        tinyWRAPPINVOKE.delete_MediaContentCPIM(swigCPtr);
      }
      swigCPtr = new HandleRef(null, IntPtr.Zero);
      GC.SuppressFinalize(this);
      base.Dispose();
    }
  }

  public override uint getPayloadLength() {
    uint ret = tinyWRAPPINVOKE.MediaContentCPIM_getPayloadLength(swigCPtr);
    return ret;
  }

  public override uint getPayload(byte[] pOutput, uint nMaxsize) {
    uint ret = tinyWRAPPINVOKE.MediaContentCPIM_getPayload(swigCPtr, pOutput, nMaxsize);
    return ret;
  }

  public string getHeaderValue(string pName) {
    string ret = tinyWRAPPINVOKE.MediaContentCPIM_getHeaderValue(swigCPtr, pName);
    return ret;
  }

}

}