import { ComponentFixture, TestBed } from '@angular/core/testing';

import { FirstLoadingComponent } from './first-loading.component';

describe('FirstLoadingComponent', () => {
  let component: FirstLoadingComponent;
  let fixture: ComponentFixture<FirstLoadingComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [FirstLoadingComponent]
    })
    .compileComponents();

    fixture = TestBed.createComponent(FirstLoadingComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
