#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "DeviceController.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setupAssignment();
    void setupAssignmentSubPart(int);
    void setupDifficultyCheckboxes(const QString& subpart);
private slots:
    void on_ipAddress_textChanged(const QString &arg1);

    void on_btnConnect_clicked();

    void device_connected();
    void device_disconnected();
    void device_stateChanged(QAbstractSocket::SocketState);
    void device_errorOccurred(QAbstractSocket::SocketError);
    void device_dataReady(QByteArray);
    void on_btnSend_clicked();

    void on_assignments_currentIndexChanged(int index);

    void on_addPassedAssignments_clicked();

    void on_assignmentSubPart_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;
    DeviceController _controller;    
    QString _subPart,_testCasesPassed,_subPartsDoneCompleteMessage;
    void setDeviceController();
    QMap<QString, QStringList> mistakeMap = {
        {"Raster Grid", {
                            "Incorrect grid initialization leading to segmentation faults.",
                            "Improper handling of pixel sizes resulting in distorted grids."
                        }},
        {"DDA", {
                                   "Off-by-one errors in endpoint calculations leading to missing pixels.",
                                   "Incorrect handling of line slopes, causing misdrawn lines."
                               }},
        {"Bresenham Line", {
                                         "Incorrect increment direction for x or y, leading to wrong line placement.",
                                         "Overflow issues when calculating pixel positions, causing crashes."
                                     }},
        {"Polar Circle", {
                                       "Wrong angle conversions leading to incorrect pixel placements.",
                                       "Improper radius handling causing incomplete circles."
                                   }},
        {"Bresenham Circle", {
                                                    "Incorrect mid-point decision logic resulting in distorted circles.",
                                                    "Off-center circles due to wrong center point initialization."
                                                }},
        {"Cartesian", {
                                           "Incorrect coordinate placements resulting in misaligned circles.",
                                           "Incorrect radius calculations leading to unexpected circle sizes."
                                       }},
        {"Midpoint", {
                                          "Circle approximation errors leading to unexpected shapes.",
                                          "Boundary miscalculations causing parts of the circle to be omitted."
                                      }},
        {"Polar Ellipse", {
                             "Wrong angle conversions leading to incorrect pixel placements.",
                             "Improper radius handling causing incomplete circles."
                         }},
        {"Bresenham Ellipse", {
                                 "Incorrect mid-point decision logic resulting in distorted circles.",
                                 "Off-center circles due to wrong center point initialization."
                             }},
        {"Scanline Fill", {
                                           "Incorrect edge sorting leading to missed fills.",
                                           "Pixel overflow issues if boundaries are not properly defined."
                                       }},
        {"Boundary Fill", {
                              "Unbounded fill areas due to improper boundary checks.",
                              "Stack overflow from deep recursion in large fill areas."
                          }},
        {"Flood Fill - 4 Connected", {
                                         "Misidentifying fill areas due to incorrect pixel connectivity checks.",
                                         "Stack overflow issues in large areas due to deep recursion."
                                     }},
        {"Flood Fill - 8 Connected", {
                                         "Fill lagging issues from inefficient algorithm design.",
                                         "Performance bottlenecks due to excessive redraws."
                                     }},
        {"Translation", {
                                "Incorrect transformation matrices causing unexpected results.",
                                "Improper handling of homogeneous coordinates leading to scaling issues."
                            }},
        {"Rotation", {
                            "Incorrect transformation matrices causing unexpected results.",
                            "Improper handling of homogeneous coordinates leading to scaling issues."
                        }},
        {"Shear", {
                            "Incorrect transformation matrices causing unexpected results.",
                            "Improper handling of homogeneous coordinates leading to scaling issues."
                        }},
        {"Scaling", {
                            "Incorrect transformation matrices causing unexpected results.",
                            "Improper handling of homogeneous coordinates leading to scaling issues."
                        }},
        {"Reflection", {
                            "Incorrect transformation matrices causing unexpected results.",
                            "Improper handling of homogeneous coordinates leading to scaling issues."
                        }},
        {"Rotation wrt Arbitrary Point", {
                                         "Errors in applying transformations in the wrong order.",
                                         "Miscalculations in arbitrary point handling resulting in wrong outputs."
                                     }},
        {"Scaling wrt Arbitrary Point", {
                                             "Errors in applying transformations in the wrong order.",
                                             "Miscalculations in arbitrary point handling resulting in wrong outputs."
                                         }},
        {"Reflection wrt Arbitrary Line", {
                                            "Errors in applying transformations in the wrong order.",
                                            "Miscalculations in arbitrary point handling resulting in wrong outputs."
                                        }},
        {"Cohen-Sutherland", {
                                                 "Incorrect outcode calculations causing clipped lines to be incorrectly represented.",
                                                 "Wrong implementation of line intersections resulting in missing segments."
                                             }},
        {"Liang-Barsky (Optional)", {
                                             "Boundary miscalculations leading to incorrect clipped lines.",
                                             "Improper handling of edge cases causing crashes or infinite loops."
                                         }},
        {"Sutherland-Hodgeman", {
                                                       "Incorrect vertex ordering resulting in improper clipping.",
                                                       "Performance issues due to inefficient edge handling."
                                                   }},
        {"Weiler-Atherton (Optional)", {
                                                   "Boundary miscalculations causing incorrect clip results.",
                                                   "Errors in handling intersection points leading to missing polygons."
                                               }},
        {"Bezier Curve", {
                             "Incorrect control point handling resulting in malformed curves.",
                             "Improper curve parameterization leading to missing segments."
                         }},
        // Add more mappings as needed
    };


};
#endif // MAINWINDOW_H
