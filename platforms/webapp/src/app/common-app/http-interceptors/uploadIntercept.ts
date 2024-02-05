import { Injectable } from '@angular/core';
import { HttpEvent, HttpInterceptor, HttpHandler, HttpRequest } from '@angular/common/http';
import { Observable } from 'rxjs/internal/Observable';
import { tap } from 'rxjs/operators';
import { getMessageId } from '../utils';
import { TimeoutService } from '../service/timeout.service';

/** Pass untouched request through to the next request handler. */
@Injectable()
export class UploadInterceptor implements HttpInterceptor {
  constructor(private timeoutService: TimeoutService) { }
  intercept(req: HttpRequest<any>, next: HttpHandler): Observable<HttpEvent<any>> {
    return next.handle(req).pipe(
      tap(
        (event) => { },
        (error) => {
          let errorId: string;
          if (getMessageId(error?.error).length > 0) {
            errorId = getMessageId(error.error)[0]?.errorId;
          }
          if (errorId === 'NoValidSession') {
            this.timeoutService.error401.next(errorId);
          }
        }
      )
    );
  }
}
